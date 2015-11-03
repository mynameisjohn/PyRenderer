#pragma once

/**********************************************
	My crazy object oriented take on IQM
	Loads in a file, gives accessors via IqmData
	Designed to be easy to use with OpenGL
	***********************************************/

// Wish I didn't have to include these
// TODO move everything in here, remove STL
#include <map>
#include <vector>
#include <stdint.h>

class IqmFile
{
public:
	enum class IQM_T : uint32_t
	{	// Various types of data within an IQM File
		POSITION = 0,
		TEXCOORD = 1,
		NORMAL = 2,
		TANGENT = 3,
		BLENDINDEXES = 4,
		BLENDWEIGHTS = 5,
		COLOR = 6,
		VARRAY = 7,
		MESH = 8,
		TRI = 9,
		JOINT = 10,
		POSE = 11,
		ANIM = 12,
		FRAME = 13,
        BBOX = 14,
		CUSTOM = 0x10
	};
private:
	struct iqmheader
	{	// The IQM Header struct, which gives easy access to data
		char magic[16];
		uint32_t version;
		uint32_t filesize;
		uint32_t flags;
		uint32_t num_text, ofs_text;
		uint32_t num_meshes, ofs_meshes;
		uint32_t num_vertexarrays, num_vertexes, ofs_vertexarrays;
		uint32_t num_triangles, ofs_triangles, ofs_adjacency;
		uint32_t num_joints, ofs_joints;
		uint32_t num_poses, ofs_poses;
		uint32_t num_anims, ofs_anims;
		uint32_t num_frames, num_framechannels, ofs_frames, ofs_bounds;
		uint32_t num_comment, ofs_comment;
		uint32_t num_extensions, ofs_extensions;
	};

	// Storage for raw IQM File bytes
	std::vector<char> m_Data;

	// Ptr to file's header struct
	iqmheader * m_Header;

	// Convenient way to locate data within the file
	struct IqmWaypoint
	{	// Byte offset and count for each IQM_T
		uint32_t num;
		uint32_t ofs;
	}; 
	std::map<IQM_T, IqmWaypoint> m_WayPoints;

	// Get waypoint from IQM type code
	IqmWaypoint getWaypoint(IQM_T c) const;
public:
	// Source constructor
	IqmFile( std::string src );

	// Get string from file (his format)
	std::string getStr( uint32_t ofs_str );

	// Protected accessor methods
protected:
	// Get number of IQM_T mapped types
	inline uint32_t getNum(IQM_T c) const{
		return getWaypoint(c).num;
	}

	// Get ptr to IQM_T mapped types
	template <typename T = uint8_t>
	inline T * getPtr(IQM_T c) const{
		IqmWaypoint wp = getWaypoint(c);
		return wp.ofs ? (T *)&m_Data[wp.ofs] : nullptr;
	}

	// Public accessor class, designed to help with OpenGL API calls
	friend class IqmAttr;
public:
	/****************************************************
		The IqmAttr class is a public accessor
		that allows data within the file to be
		accessed either in its Native data format
		or some composition/decomposition.
		Example: IqmAttr<iqmposition, IQM_POSITION, float>
		allows a buffer of N iqmposition structs (each three floats)
		to be accessed as a buffer of 3*N floats.
		*****************************************************/
	template <typename N, IQM_T C, typename T = N>
	class IqmAttr
	{
		const IqmFile& m_File;
	protected:
		// Only IqmFile can construct
		friend class IqmFile;
		IqmAttr( IqmFile& file )
			: m_File( file )
		{	// Make sure the data size of T and N work out such that no data is left out
			static_assert( !( sizeof( T ) % sizeof( N ) ) || !( sizeof( N ) % sizeof( T ) ), "IQM Error: In order to create an IqmAttr, the Native Type must divide evenly into sizeof(N) / sizeof(T) units, or vice versa." );
		}
		
		inline uint32_t ratio() const
		{	// T per N, or N per T
			return ( sizeof( T ) > sizeof( N ) ? sizeof( T ) / sizeof( N ) : sizeof( N ) / sizeof( T ) );
		}
	public:
		// The count of this data type within the file
		inline uint32_t count() const
		{
			return ratio() * m_File.getNum( C );
		}
		// The size of the data in bytes
		inline uint32_t numBytes() const
		{
			return count() * sizeof( T );
		}
		// Type Size
		inline size_t size() const
		{
			return sizeof( T );
		}
		// Native Size
		inline size_t nativeSize() const
		{
			return sizeof( N );
		}
		// Pointer to location in file
		inline T * ptr() const
		{
			return m_File.getPtr<T>( C );
		}
		// Array operator
		inline T& operator[](const uint32_t idx) const
		{
			return idx < count() ? ptr()[idx] : ptr()[count() - 1];
		}
		// Copy to a new vector
		inline std::vector<T> toVec() const
		{
			std::vector<T> ret( count() );
			memcpy_s( ret.data(), numBytes(), ptr(), numBytes() );
			ret.shrink_to_fit();
			return ret;
		}
	};
	// Get Attr; can return attr of different type than native, provided the strides work out
	template <typename N, IQM_T C, typename T = N>
	inline IqmAttr<N, C, T> getAttr()
	{
		return IqmAttr<N, C, T>( *this );
	}

	// Various structs used in IQM, left public
	struct iqmmesh
	{
		unsigned int name;
		unsigned int material;
		unsigned int first_vertex, num_vertexes;
		unsigned int first_triangle, num_triangles;
	};

	struct iqmjoint
	{
		unsigned int name;
		int parent;
		float translate[3], rotate[4], scale[3];
	};

	struct iqmpose
	{
		int parent;
		unsigned int mask;
		float channeloffset[10];
		float channelscale[10];
	};

	struct iqmanim
	{
		unsigned int name;
		unsigned int first_frame, num_frames;
		float framerate;
		unsigned int flags;
	};

	enum
	{
		IQM_LOOP = 1 << 0
	};

	struct iqmbounds
	{
		float bbmin[3], bbmax[3];
		float xyradius, radius;
	};

	// Indices (triangles)
	struct iqmtriangle{ uint32_t a, b, c; };

	// Vertex attributes
	struct iqmposition{ float x, y, z; };
	struct iqmtexcoord{ float u, v; };
	struct iqmnormal{ float nX, nY, nZ; };
	struct iqmtangent{float tX, tY, tZ, tW;};
	struct iqmblendidx{ uint8_t bI[4]; };
	struct iqmblendweight{ uint8_t bW[4]; };

	// structs and enums needed when loading the file
private:
	enum class IQM_P : uint32_t
	{	// Primitive Types
		BYTE = 0,
		UBYTE = 1,
		SHORT = 2,
		USHORT = 3,
		INT = 4,
		UINT = 5,
		HALF = 6,
		FLOAT = 7,
		DOUBLE = 8
	};

	struct iqmvertexarray
	{	// vertex array in the file
		uint32_t type;
		uint32_t flags;
		uint32_t format;
		uint32_t size;
		uint32_t offset;
	};

	// Convenient public access functions
public:
	// I think I can declare these with a template rather than a macro... generates a specific getAttr funcion
#define IQMATTRFNGENMACRO(N,C,fn) template <typename T = N> inline IqmAttr<N, C, T> fn(){ return getAttr<N, C, T>(); }
	// Returns Position Attr, a float3 type
	IQMATTRFNGENMACRO( iqmposition, IQM_T::POSITION, Positions );
	// Returns Tex Coord Attr, a float2 type
	IQMATTRFNGENMACRO( iqmtexcoord, IQM_T::TEXCOORD, TexCoords );
	// Returns Normals Attr, a float3 type
	IQMATTRFNGENMACRO( iqmnormal, IQM_T::NORMAL, Normals );
	// Returns Tangents Attr, a float4 type
	IQMATTRFNGENMACRO( iqmtangent, IQM_T::TANGENT, Tangents );
	// Returns Blend Indices Attr, a uchar4 type
	IQMATTRFNGENMACRO( iqmblendidx, IQM_T::BLENDINDEXES, BlendIndices );
	// Returns Blend Weights Attr, a uchar4 type
	IQMATTRFNGENMACRO( iqmblendweight, IQM_T::BLENDWEIGHTS, BlendWeights );
	// Returns Meshes
	IQMATTRFNGENMACRO( iqmmesh, IQM_T::MESH, Meshes );
	// Returns Triangles (Geometry Indices)
	IQMATTRFNGENMACRO( iqmtriangle, IQM_T::TRI, Triangles );
	// Returns Joints
	IQMATTRFNGENMACRO( iqmjoint, IQM_T::JOINT, Joints );
	// Returns anims
	IQMATTRFNGENMACRO( iqmanim, IQM_T::ANIM, Anims );
	// Returns frames
	IQMATTRFNGENMACRO( uint16_t, IQM_T::FRAME, Frames );
	// Returns triangles as uint32_t rather than iqmtriangle
	auto Indices()->decltype( Triangles<uint32_t>() ) { return Triangles<uint32_t>(); }
	// Get # of frames
	inline uint32_t getNumFrames() { return m_Header->num_frames; }
};

