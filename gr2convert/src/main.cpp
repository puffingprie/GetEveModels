
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "granny.h"
#include "half.cpp"
#include "half.h"

void __cdecl log(int a, int b, char* msg)
{
	fprintf(stderr, "Log (%d %d) %s\n", a, b, msg);
}

GrannyLogger logger = { &log, &log };

int getTypeSize(int type) {
	switch(type) {
		case 10: return 4;  // 32bit float
		case 11:            // granny_int8
		case 12:            // granny_uint8
		case 14: return 1;  // granny_uint8
		case 21: return 2;  // 16bit float
		default: return 0;
	}
}

typedef float (*convertFunc)(uint8_t* value);

float convertDummy(uint8_t* value) {
	return 0.0f;
}

float convert10(uint8_t* value) {
	return *(float*)value;
}

float convert11(uint8_t* value) {
	return *(granny_int8*)value;
}

float convert12or14(uint8_t* value) {
	return *(granny_uint8*)value;
}

float convert21(uint8_t* value) {
	uint32_t result = half_to_float(*(uint16_t*)value);
	return *(float*)&result;
}

convertFunc getConvertFunc(int type) {
	switch (type) {
		case 10: return &convert10;
		case 11: return &convert11;
		case 12:
		case 14: return &convert12or14;
		case 21: return &convert21;
		default: return &convertDummy;
	}
}

struct VertexComponent {
	const char* name;     // name of the component as used in granny files
	int32_t index;        // index of -1 means it doesn't exist
	bool exists;          // does this component exit
	convertFunc convert;  // convertion function to convert from this components type to 32bit float
	uint32_t offset;      // offset relative to the start of the vertex at which this component starts
	uint32_t typeSize;    // size in bytes of the type of this component
};

uint32_t nVertexComponents_ = 6;

VertexComponent vertexComponents_[] = {
	{ "Position"           , -1, false, &convertDummy, 0, 0 },
	{ "Normal"             , -1, false, &convertDummy, 0, 0 },
	{ "Tangent"            , -1, false, &convertDummy, 0, 0 },
	{ "Binormal"           , -1, false, &convertDummy, 0, 0 },
	{ "TextureCoordinates0", -1, false, &convertDummy, 0, 0 },
	{ "TextureCoordinates1", -1, false, &convertDummy, 0, 0 }
};

int nTriangleGroups_;
Group_t* triangleGroups_;

int bytesPerVertex_;
int nVertices_;
uint8_t* vertices_;

int bytesPerIndex_;
int nIndices_;
uint8_t* indices_;

void getMeshInfo(const Mesh_t* mesh) {
	// get amount of TriangleGroups (Surfaces)
	nTriangleGroups_ = _GrannyGetMeshTriangleGroupCount(mesh);
	triangleGroups_ = _GrannyGetMeshTriangleGroups(mesh);

	StringMember_t* sm = mesh->PrimaryVertexData->Vertices_strings;


	uint32_t currentOffset = 0;

	// get amount of vertex components
	// _GrannyGetVertexComponentCount(sm); doesnt work as TextureCoordinates0
	// and TextureCoordinates1 is counted as one component
	// uint32_t nVertexComponents = _GrannyGetVertexComponentCount(sm);
	uint32_t nVertexComponents = 0;

	for(int i = 0; sm[i].Type; ++i)
		++nVertexComponents;

	// offset for each component in the vertex structure
	uint32_t* offsets = new uint32_t[nVertexComponents];

	// calculate offset for each component
	for(uint32_t iVertexComponent = 0; iVertexComponent < nVertexComponents; ++iVertexComponent) {
		offsets[iVertexComponent] =  currentOffset;
		currentOffset += getTypeSize(sm[iVertexComponent].Type) * sm[iVertexComponent].Count;
	}

	bytesPerVertex_ = currentOffset;

	// set data for each component
	for(uint32_t i = 0; i < nVertexComponents_; ++i) {
		vertexComponents_[i].index    = _GrannyGetVertexComponentIndex(vertexComponents_[i].name, sm);
		vertexComponents_[i].exists   = vertexComponents_[i].index != -1;
		vertexComponents_[i].convert  = (vertexComponents_[i].exists) ? getConvertFunc(sm[vertexComponents_[i].index].Type) : &convertDummy;
		vertexComponents_[i].offset   = (vertexComponents_[i].exists) ? offsets[vertexComponents_[i].index] : 0;
		vertexComponents_[i].typeSize = (vertexComponents_[i].exists) ? getTypeSize(sm[vertexComponents_[i].index].Type) : 0;
	}

	delete[] offsets;

	nVertices_ = _GrannyGetMeshVertexCount(mesh);
	vertices_ = (uint8_t*)_GrannyGetMeshVertices(mesh);

	bytesPerIndex_ = _GrannyGetMeshBytesPerIndex(mesh);
	nIndices_ = _GrannyGetMeshIndexCount(mesh);
	indices_ = (uint8_t*)_GrannyGetMeshIndices(mesh);

}

void convertMeshToObj(const Mesh_t* mesh, FILE* file) {
	fprintf(stderr, "# VertexCount: %d, BytesPerVertex: %d\n", nVertices_, bytesPerVertex_);
	fprintf(stderr, "# IndexCount: %d, BytesPerIndex: %d\n", nIndices_, bytesPerIndex_);

	if(vertexComponents_[0].exists) {
		printf("--- Position found %d \n", nVertices_);
		for(int iVertex = 0; iVertex < nVertices_; ++iVertex) {
			uint8_t* vertex = vertices_ + bytesPerVertex_ * iVertex + vertexComponents_[0].offset;
			fprintf(file, "v %f %f %f\n",
			vertexComponents_[0].convert(vertex + 0 * vertexComponents_[0].typeSize),
			vertexComponents_[0].convert(vertex + 1 * vertexComponents_[0].typeSize),
			vertexComponents_[0].convert(vertex + 2 * vertexComponents_[0].typeSize)
			);
		}
	}

	if(vertexComponents_[1].exists) {
	    printf("--- Normals found\n");
		for(int iVertex = 0; iVertex < nVertices_; ++iVertex) {
			uint8_t* vertex = vertices_ + bytesPerVertex_ * iVertex + vertexComponents_[1].offset;
			fprintf(file, "vn %f %f %f\n",
			vertexComponents_[1].convert(vertex + 0 * vertexComponents_[1].typeSize) / 255,
			vertexComponents_[1].convert(vertex + 1 * vertexComponents_[1].typeSize) / 255,
			vertexComponents_[1].convert(vertex + 2 * vertexComponents_[1].typeSize) / 255
			);
		}
	}

	if(vertexComponents_[4].exists) {
		printf("--- Texture coordinates 0 found\n");
		for(int iVertex = 0; iVertex < nVertices_; ++iVertex) {
			uint8_t* vertex = vertices_ + bytesPerVertex_ * iVertex + vertexComponents_[4].offset;
			fprintf(file, "vt %f %f\n",
			vertexComponents_[4].convert(vertex + 0 * vertexComponents_[4].typeSize),
			vertexComponents_[4].convert(vertex + 1 * vertexComponents_[4].typeSize)
			);
		}
	}

	granny_uint32 index0;
	granny_uint32 index1;
	granny_uint32 index2;
	granny_uint32 index3;
	
	printf("--- Faces found %d (bytes per index: %d) \n", nTriangleGroups_, bytesPerIndex_);

	if(bytesPerIndex_ == 2) {
        uint16_t* indices = (uint16_t*) indices_;

        for(int iGroup = 0; iGroup < nTriangleGroups_; ++iGroup) { // for each surface
            fprintf(file, "g area_%d\n", triangleGroups_[iGroup].MaterialIndex);
            //fprintf(file, "g %s\n", mesh->MaterialBindings[triangleGroups_[iGroup].MaterialIndex].Material->Name);
            for(int iTriangle = 0; iTriangle < triangleGroups_[iGroup].TriCount; ++iTriangle) {
                index0 = (triangleGroups_[iGroup].TriFirst + iTriangle) * 3;
                index1 = indices[index0 + 0] + 1;
                index2 = indices[index0 + 1] + 1;
                index3 = indices[index0 + 2] + 1;
                fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                index1, index1, index1,
                index2, index2, index2,
                index3, index3, index3);
            }
        }
    }
    else if(bytesPerIndex_ == 4) {
        uint32_t* indices = (uint32_t*) indices_;

        for(int iGroup = 0; iGroup < nTriangleGroups_; ++iGroup) { // for each surface
            //fprintf(file, "g %s\n", mesh->MaterialBindings[triangleGroups_[iGroup].MaterialIndex].Material->Name);
            fprintf(file, "g area_%d\n", triangleGroups_[iGroup].MaterialIndex);
            for(int iTriangle = 0; iTriangle < triangleGroups_[iGroup].TriCount; ++iTriangle) {
                index0 = (triangleGroups_[iGroup].TriFirst + iTriangle) * 3;
                index1 = indices[index0 + 0] + 1;
                index2 = indices[index0 + 1] + 1;
                index3 = indices[index0 + 2] + 1;
                fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                index1, index1, index1,
                index2, index2, index2,
                index3, index3, index3);
            }
        }
    }
}


int main(int argc, char** argv) {
	if(argc < 3) {
		fprintf(stdout, "Usage: %s <src> <dst>\n", argv[0]);
		exit(1);
	}

	granny_int32 result;

	fprintf(stderr, "Granny Version: %s\n", _GrannyGetVersionString());
	result = _GrannySetLogCallback(&logger);

	GrannyFile_t* grannyFile = _GrannyReadEntireFile(argv[1]);
	if(!grannyFile) {
		printf("Could not load %s\n", argv[1]);
		exit(1);
	}

	FileInfo_t* fileInfo = _GrannyGetFileInfo(grannyFile);
	if(!fileInfo) {
		printf("Could not get fileinfo\n");
		exit(1);
	}

	printf("%d\n", fileInfo);
	// Get model count
	granny_int32 nModels = fileInfo->nModels;
	printf("Models found: %d \n", nModels);
	// Get mesh count
	granny_int32 nMeshes = fileInfo->nMeshes;
	printf("Meshes found: %d \n", nMeshes);
	printf("\n");
	
	if(!fileInfo->nMeshes) {
		printf("Could not find any meshes in %s\n", argv[1]);
		exit(1);
	}

	// Open file
	FILE* file = fopen(argv[2], "w");

	// Loop Meshes
	for(int iMesh = 0; iMesh < nMeshes; ++iMesh) {
		Mesh_t* mesh = fileInfo->Meshes[iMesh];
		char* name = mesh->Name;

		printf("- Mesh: %s (%d)\n", name, iMesh);

		getMeshInfo(mesh);
		convertMeshToObj(mesh, file);
		printf("\n");
	}

	// Close file
	fclose(file);

	_GrannyFreeFile(grannyFile);
	return 0;
}
