#include <mgdl-wii.h>
#include <GL/opengx.h>

#include "tunerfx.h"
#include "FontGL.h"

void TunerFx::Init()
{

}

void TunerFx::Update ( float deltaTime )
{
	// Move the orange line
	linePos += deltaTime;
}


void TunerFx::Draw ( FontGL* font )
{
	// Draw the border frame
	font->Printf(1.0f, gdl::LJustify, gdl::LJustify, "TEST");



	// Draw the guide lines


	// Draw found names


	// Draw the orange line
}
