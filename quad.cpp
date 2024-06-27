#include "quad.h"

void QuadMesh::Init(float z)
{
	u32 amount = 4;
	positionsArray = (gdl::wii::VERT3f32*)memalign(32, sizeof(gdl::wii::VERT3f32) * amount);
	colorsArray = (RGB8*)memalign(32, sizeof(RGB8) * amount );

	u32 listSizePadded = amount * (sizeof(u16)+sizeof(u16)) + 32; // List must be 32 bytes bigger than needed
	listPtr = memalign(32, listSizePadded);
	DCInvalidateRange(listPtr, listSizePadded);

	positionsArray[0] = {-1.0f, 1.0f, z};
	positionsArray[1] = {1.0f, 1.0f, z};
	positionsArray[2] = {1.0f, -1.0f, z};
	positionsArray[3] = {-1.0f, -1.0f, z};

	for (u32 i = 0; i < amount; i++)
	{
		colorsArray[i] = RGB8{255, 128, 255};
	}

	GX_BeginDispList(listPtr, listSizePadded);
	GX_Begin(GX_QUADS, GX_VTXFMT1, amount);
		for(u32 i = 0; i < amount; i++)
		{
			// Since the list is 32 byte aligned, both indices can be u16
			GX_Position1x16(i);
			GX_Color1x16(i); // Change by speed?
		}
	GX_End();
	displayListSize = GX_EndDispList();
	gdl_assert(displayListSize > 0 && displayListSize < listSizePadded, "Failed to create quad displayList");
	printf("Created quad list size %u < %u\n", displayListSize, listSizePadded);
}

void QuadMesh::Draw()
{
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
	GX_SetArray(GX_VA_POS, (void*)positionsArray, sizeof(gdl::wii::VERT3f32));
	GX_SetArray(GX_VA_CLR0, (void*)colorsArray, sizeof(RGB8));

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	// Concat all matrices
	Mtx modelViewMatrix;
	guMtxConcat(gdl::wii::ViewMtx, gdl::wii::ModelMtx, modelViewMatrix);
	GX_LoadPosMtxImm(modelViewMatrix, GX_PNMTX0);

	GX_CallDispList(listPtr, displayListSize);
}
