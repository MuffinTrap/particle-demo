#include "rocketdebug.h"

#include <cmath>
#include "crossOpenGL.h"
#include "crossTypedefs.h"
#include "FontGL.h"
#include "palette.h"

#include "src/direction.hpp"

void RocketDebug::Draw ( FontGL* font )
{
	glPushMatrix();

	// What is the row
	glTranslatef(-0.9f, -0.9f, 0.0f);
	u32 rowX = static_cast<u32>(floor(get_row()));
	font->Printf(WHITE, 0.1f, LJustify, LJustify, "Row: %x", rowX);

	// What is elapsed
	glTranslatef(0.0f, 0.1f, 0.0f);
	font->Printf(WHITE, 0.1f, LJustify, LJustify, "Time: %.2f", getTime());
	glPopMatrix();
}
