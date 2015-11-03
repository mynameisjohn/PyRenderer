#include "IqmFile.h"

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

/*************************************************
	Source Constructor
	Takes in binary file and parses it, storing the
	raw bytes as well as useful byte offsets and
	data type counts within the file.
	**************************************************/
IqmFile::IqmFile( string src )
{
	// Useful lambdas
	auto isLittleEndian = [] ()
	{	// Check if lil endian (not handled yet)
		union { int i; uint8_t b[sizeof( int )]; } conv;
		conv.i = 1;
		return conv.b[0] != 0;
	};
	
	// Assert with message
	auto IQMASSERT = [] ( bool cond, string msg )
	{	// Assert with message
		if ( cond == false )
		{
			cout << msg << endl;
			exit( -1 );
		}
	};

	// Version/Magic at the time of writing this
	const uint32_t IQM_VERSION = 2;
	const string IQM_MAGIC = "INTERQUAKEMODEL";

	// Read the binary file, make sure we actually get data
	ifstream vIn(src, ios::binary );
	m_Data = vector<char>( istreambuf_iterator<char>( vIn ), istreambuf_iterator<char>() );
	IQMASSERT( m_Data.size() > IQM_MAGIC.size(), "Error: No data loaded" );

	// If !littleEndian, reverse all bytes read in (NOT TESTED)
	if ( isLittleEndian() == false )
		reverse( m_Data.begin(), m_Data.end() );

	// No more data needed, save / grab what we can
	m_Data.shrink_to_fit();
	m_Header = (iqmheader *) ( m_Data.data() );

	// Checks
	IQMASSERT( m_Header != nullptr, "No IQM Header loaded" );
	IQMASSERT( m_Header->version == IQM_VERSION, "IQM file version incorrect" );
	IQMASSERT( string( m_Header->magic ) == IQM_MAGIC, "IQM File contained wrong magic number" );

	// Grab these things
    auto maybeAdd = [this](IQM_T ID, uint32_t num, uint32_t ofs){
        if (num && ofs)
            m_WayPoints[ID] = {num, ofs};
    };
    maybeAdd(IQM_T::MESH, m_Header->num_meshes, m_Header->ofs_meshes);
    maybeAdd(IQM_T::TRI, m_Header->num_triangles, m_Header->ofs_triangles);
    maybeAdd(IQM_T::JOINT, m_Header->num_joints, m_Header->ofs_joints);
    maybeAdd(IQM_T::POSE, m_Header->num_poses, m_Header->ofs_poses);
    maybeAdd(IQM_T::ANIM, m_Header->num_anims, m_Header->ofs_anims);
    maybeAdd(IQM_T::VARRAY, m_Header->num_vertexarrays, m_Header->ofs_vertexarrays);
    maybeAdd(IQM_T::FRAME, m_Header->num_frames, m_Header->ofs_frames);
    maybeAdd(IQM_T::BBOX, 1, m_Header->ofs_bounds);

	// Loop through all vertex arrays
	iqmvertexarray * vArrs( (iqmvertexarray *) &m_Data[m_Header->ofs_vertexarrays] );
	for ( uint32_t i = 0; i < m_Header->num_vertexarrays; i++ )
	{	// Check array type, cache info
		iqmvertexarray & va( vArrs[i] );
		IQM_T type = (IQM_T)va.type;
		IQM_P prim = (IQM_P)va.format;
		switch ( type )
		{	// If it's a vertex attribute, make sure it's the right primitive type and cache it
			case IQM_T::POSITION:
			case IQM_T::NORMAL:
			case IQM_T::TANGENT:
			case IQM_T::TEXCOORD:
				IQMASSERT( prim == IQM_P::FLOAT, "Error: Type of vertex attribute incorrect, expected a float" );
				break;
			case IQM_T::BLENDINDEXES:
			case IQM_T::BLENDWEIGHTS:
				IQMASSERT( prim == IQM_P::UBYTE, "Error: Type of vertex attribute incorrect, expected a byte" );
				break;
			default:
				// Is this OK?
				continue;
		}
		m_WayPoints[type] = { m_Header->num_vertexes, va.offset };
	}
}

// Get text string from IQM File
string IqmFile::getStr( uint32_t ofs_str )
{
	return ( ofs_str < m_Header->num_text ) ? string( &m_Data[m_Header->ofs_text + ofs_str] ) : string();
}

// Return waypoint from map, if it exists
IqmFile::IqmWaypoint IqmFile::getWaypoint(IQM_T c) const
{
	auto it = m_WayPoints.find( c );
	if ( it == m_WayPoints.end() )
	{
		cout << "Unable to find IQM type with code " << uint32_t(c) << endl;
		return{ 0, 0 };
	}
	return it->second;
}