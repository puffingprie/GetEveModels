#ifndef _GRANNY_H_
#define _GRANNY_H_

typedef char           granny_int8;
typedef unsigned char  granny_uint8;
typedef short          granny_int16;
typedef unsigned short granny_uint16;
typedef int            granny_int32;
typedef unsigned int   granny_uint32;
typedef float          granny_real32;

struct Granny_Int16_t {
	granny_int16 Int16;
};

struct Granny_Int32_t {
	granny_int32 Int32;
};

struct Granny_Real_t {
	granny_real32 Real32;
};

struct granny_transform { float a; float b[3]; float c[4]; float d[3][3]; }; // is this correct ?


struct StringMember_t {
	granny_int32 Type;
	char* Name;
	void* StructOffset;
	int Count;
	int x[4];
};

struct ArtToolInfo_t {
	char* FromArtToolName;
	granny_int32 ArtToolMajorRevision;
	granny_int32 ArtToolMinorRevision;
	granny_real32 UnitsPerMeter;
	granny_real32 Origin[3];
	granny_real32 RightVector[3];
	granny_real32 UpVector[3];
	granny_real32 BackVector[3];
	void* ExtendedData;
};

struct ExporterInfo_t {
	char* ExporterName;
	granny_int32 ExporterMajorRevision;
	granny_int32 ExporterMinorRevision;
	granny_int32 ExporterCustomization;
	granny_int32 ExporterBuildNumber;
};

struct Layout_t {
	granny_int32 BytesPerPixel;
	granny_int32 ShiftForComponent[4];
	granny_int32 BitsForComponent[4];
};

struct PixelByte_t {
	granny_uint8 UInt8;
};

struct MIPLevel_t {
	granny_int32 Stride;
	granny_int32 nPixelBytes;
	PixelByte_t* PixelBytes;
};

struct Image_t {
	granny_int32 nMIPLevels;
	MIPLevel_t* MIPLevels;
};

struct Texture_t {
	char* FromFileName;
	granny_int32 TextureType;
	granny_int32 Width;
	granny_int32 Height;
	granny_int32 Encoding;
	granny_int32 SubFormat;
	Layout_t Layout;
	granny_int32 nImages;
	Image_t* Images;
	void* ExtendedData;
};

struct Material_t;

struct Maps_t {
	char* Usage;
	Material_t* Map;
};

struct Material_t {
	char* Name;
	granny_int32 nMaps;
	Maps_t* Maps;
	Texture_t* Texture;
	void* ExtendedData;
};

// better not use is as it is not known if granny_transform is correct
struct Bone_t {
	char* Name;
	granny_int32 ParentIndex;
	granny_transform Transform;
	granny_real32 InverseWorldTransform[16];
	granny_real32 LODError;
	void* ExtendedData;
//	LightInfo_t* LightInfo; //  not in GrannyViewer
//	CameraInfo_t* CameraInfo; // not in GrannyViewer
};


struct Skeleton_t {
	char* Name;
	granny_int32 nBones;
	Bone_t* Bones;
	granny_int32 LODType;
};

struct VertexComponentName_t {
	char* String;
};

typedef Granny_Int32_t VertexAnnotationIndex_t;

// is this correct
struct VertexAnnotationSet_t {
	char* Name;
	granny_int32 nVertexAnnotations;
	void * VertexAnnotations;
	granny_int32 nVertexAnnotationIndices;
	VertexAnnotationIndex_t* VertexAnnotationIndices;
//	t_StringMember * VertexAnnotations_strings;
};

// is this correct
struct VertexData_t {
	StringMember_t* Vertices_strings; // not in GrannyViewer
	granny_int32 nVertices;
	void* Vertices;
	granny_int32 nVertexComponentNames;
	VertexComponentName_t* VertexComponentNames;
	granny_int32 nVertexAnnotationSets;
	VertexAnnotationSet_t * VertexAnnotationSets;
};


struct Group_t {
	granny_int32 MaterialIndex;
	granny_int32 TriFirst;
	granny_int32 TriCount;
};



typedef Granny_Int32_t TriAnnotationIndex_t;

// is this correct
struct TriAnnotationSet_t {
	char* Name;
	granny_int32 nTriAnnotations;
	void* TriAnnotations;
	granny_int32 nTriAnnotationIndices;
	TriAnnotationIndex_t* TriAnnotationIndices;
//	t_StringMember* TriAnnotations_strings;
};

typedef Granny_Int32_t Index_t;
typedef Granny_Int16_t Index16_t;
typedef Granny_Int32_t VertexToVertexMap_t;
typedef Granny_Int32_t VertexToTriangleMap_t;
typedef Granny_Int32_t SideToNeighborMap_t;
typedef Granny_Int32_t BonesForTriangle_t;
typedef Granny_Int32_t TriangleToBoneIndex_t;

struct TriTopology_t {
	granny_int32 nGroups;
	Group_t* Groups;

	granny_int32 nIndices;
	Index_t* Indices;

	granny_int32 nIndices16;
	Index16_t* Indices16;

	granny_int32 nVertexToVertexMap;
	VertexToVertexMap_t* VertexToVertexMap;

	granny_int32 nVertexToTriangleMap;
	VertexToTriangleMap_t* VertexToTriangleMap;

	granny_int32 nSideToNeighborMap;
	SideToNeighborMap_t* SideToNeighborMap;

	granny_int32 nBonesForTriangle;
	BonesForTriangle_t* BonesForTriangle;

	granny_int32 nTriangleToBoneIndices;
	TriangleToBoneIndex_t* TriangleToBoneIndices;

	granny_int32 nTriAnnotationSets;
	TriAnnotationSet_t* TriAnnotationSets;
};

struct MorphTarget_t {
	char* ScalarName;
	VertexData_t* VertexData;
};

struct MaterialBinding_t {
	Material_t* Material;
};

typedef Granny_Int32_t TriangleIndex_t;

struct BoneBinding_t {
	char* BoneName;
	granny_real32 OBBMin[3];
	granny_real32 OBBMax[3];
	granny_int32 nTriangleIndices;
	TriangleIndex_t* TriangleIndices;
};

struct Mesh_t {
	char* Name;
	VertexData_t* PrimaryVertexData;
	granny_int32 nMorphTargets;
	MorphTarget_t* MorphTargets;
	TriTopology_t* PrimaryTopology;
	granny_int32 nMaterialBindings;
	MaterialBinding_t* MaterialBindings;
	granny_int32 nBoneBindings;
	BoneBinding_t* BoneBindings;
	void* ExtendedData;
};

struct MeshBinding_t {
	Mesh_t* Mesh;
};

struct Model_t {
	char* Name;
	Skeleton_t* Skeleton;
	granny_transform InitialPlacement;
	granny_int32 nMeshBindings;
	MeshBinding_t** MeshBindings;
};

typedef void VectorTrack_t; // unknown

struct CurveDataHeader_t {
	granny_uint8 Format;
	granny_uint8 Degree;
};

typedef Granny_Real_t Knot_t;
typedef Granny_Real_t Control_t;

struct CurveData_t {
	CurveDataHeader_t CurveDataHeader_DaK32fC32f_t;
	granny_int16 Padding;
	granny_int32 nKnots;
	Knot_t* Knots;
	granny_int32 nControls;
	Control_t* Controls;
};

struct Curve_t {
	CurveData_t CurveData;
};

typedef Curve_t OrientationCurve_t;
typedef Curve_t PositionCurve_t;
typedef Curve_t ScaleShearCurve_t;

struct TransformTrack_t {
	char* Name;
	granny_int32 Flags;
	OrientationCurve_t OrientationCurve;
	PositionCurve_t PositionCurve;
	ScaleShearCurve_t ScaleShearCurve;
};

typedef void TransformLODTrack_t; // unknown

struct Entry_t {
	granny_real32 TimeStamp;
	char* Text;
};

struct TextTrack_t {
	char* Name;
	granny_int32 nEntries;
	Entry_t* Entries;
};

struct PeriodicLoop_t {
	granny_real32 Radius;
	granny_real32 dAngle;
	granny_real32 dZ;
	granny_real32 BasisX[3];
	granny_real32 BasisY[3];
	granny_real32 Axis[3];
};

typedef void RootMotion_t; //unknown

struct TrackGroup_t {
	char* Name;

	granny_int32 nVectorTracks;
	VectorTrack_t* VectorTracks;

	granny_int32 nTransformTracks;
	TransformTrack_t* TransformTracks;

	granny_int32 nTransformLODTracks;
	TransformLODTrack_t* TransformLODTracks;

	granny_int32 nTextTracks;
	TextTrack_t* TextTracks;

	granny_transform InitialPlacement;
	granny_int32 AccumulationFlags;
	granny_real32 LoopTranslation[3];
	PeriodicLoop_t* PeriodicLoop;
	RootMotion_t* RootMotion;
	void* ExtendedData;
};

struct Animation_t {
	char* Name;
	granny_real32 Duration;
	granny_real32 TimeStep;
	granny_real32 Oversampling;
	granny_int32 nTrackGroups;
	TrackGroup_t** TrackGroups; /* VariantReferenceMember */
};


struct FileInfo_t {
	ArtToolInfo_t* ArtToolInfo;
	ExporterInfo_t* ExporterInfo;
	char* FromFileName;
	granny_int32 nTextures;
	Texture_t** Textures;
	granny_int32 nMaterials;
	Material_t** Materials;
	granny_int32 nSkeletons;
	Skeleton_t** Skeletons;
	granny_int32 nVertexDatas;
	VertexData_t** VertexDatas;
	granny_int32 nTriTopologies;
	TriTopology_t** TriTopologies;
	granny_int32 nMeshes;
	Mesh_t** Meshes;
	granny_int32 nModels;
	Model_t** Models;
	granny_int32 nTrackGroups;
	TrackGroup_t** TrackGroups;
	granny_int32 nAnimations;
	Animation_t** Animations;
};

// how is this structure
struct GrannyFile_t {
	char unknown01[4];
	granny_uint32* filerevision; // ?
	void* unknown012;
	granny_uint32 sections; // ?
	void* uknown02;
	void* uknown03;
	void* uknown04;
	FileInfo_t* fileInfo;
	char unknownFileHeader[32];
	ArtToolInfo_t** unknown05;
	char unknown[28];
};

struct GrannyFile;

struct GrannyLogger
{
	void (__cdecl *a) (int a, int b, char* msg);
	void (__cdecl *b) (int a, int b, char* msg);
};

#ifdef __cplusplus
extern "C" {
#endif

// not sure if there is a return type
granny_int32  __stdcall _GrannyCopyMeshIndices(const Mesh_t*, granny_int32 size, void* dst);
// not sure if there is a return type
granny_int32  __stdcall _GrannyCopyMeshVertices(const Mesh_t* mesh, granny_int32 type, void* dst);
// not sure if there is a return type
granny_int32  __stdcall _GrannyConvertFileToRaw(const char* src, const char* dst);

granny_int32  __stdcall _GrannyFreeFile(GrannyFile_t*);
FileInfo_t*   __stdcall _GrannyGetFileInfo(const GrannyFile_t*);
granny_int32  __stdcall _GrannyGetFileTypeTag(const GrannyFile_t*);

//unknown parameters
//char*         __stdcall _GrannyGetMemberTypeName(const StringMember_t* sm);
//granny_int32  __stdcall _GrannyGetMemberTypeSize(const StringMember_t* sm);

granny_int32  __stdcall _GrannyGetMeshBytesPerIndex(const Mesh_t*);
granny_int32  __stdcall _GrannyGetMeshIndexCount(const Mesh_t* mesh);
void*         __stdcall _GrannyGetMeshIndices(const Mesh_t*);

granny_int32  __stdcall _GrannyGetMeshTriangleCount(const Mesh_t* mesh);
granny_int32  __stdcall _GrannyGetMeshTriangleGroupCount(const Mesh_t* mesh);
Group_t*      __stdcall _GrannyGetMeshTriangleGroups(const Mesh_t* mesh);

granny_int32  __stdcall _GrannyGetMeshVertexCount(const Mesh_t* mesh);
granny_int32* __stdcall _GrannyGetMeshVertexType(const Mesh_t* mesh);
void*         __stdcall _GrannyGetMeshVertices(const Mesh_t* mesh);

// sm = Mesh->PrimaryVertexData->Vertices_strings
granny_int32  __stdcall _GrannyGetVertexComponentCount(const StringMember_t* sm);
// returns -1 if not found
granny_int32  __stdcall _GrannyGetVertexComponentIndex(const char* name, const StringMember_t* sm);
//unknown
//const char*   __stdcall _GrannyGetVertexComponentToolName(const char* name, const StringMember_t* sm);

const char*   __stdcall _GrannyGetVersionString();
GrannyFile_t* __stdcall _GrannyReadEntireFile(const char* filename);
GrannyFile_t* __stdcall _GrannyReadEntireFileFromMemory(granny_int32 size, const void* src);
//not sure if there is a return type
granny_int32  __stdcall _GrannySetLogCallback(GrannyLogger* logger);

#ifdef __cplusplus
}
#endif

#endif

