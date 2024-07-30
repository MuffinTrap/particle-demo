#include "titlefx.h"

#include "crossOpenGL.h"
#include "palette.h"


#include "FontGL.h"

#include "rocket/track.h"
#include "rocket/sync.h"
#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *title_rotY;  // rotation of the symbol
#else
	#include "src/sync_data.cpp"
	#include "src/sync_data.h"
#endif

void TitleFX::Init(sync_device* rocket)
{
	crossPattern = "$ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $";
#ifndef SYNC_PLAYER
	title_rotY = sync_get_track(rocket, "title_rotY");
#endif
}

void TitleFX::Save()
{
#ifndef SYNC_PLAYER
	save_sync(title_rotY, SYNC_FILE_H, SYNC_FILE_CPP);
#endif
}

void TitleFX::Draw ( FontGL* font )
{
	glPushMatrix();
		glTranslatef(-0.7f, 0.0f, 0.0f);
		glRotatef(sync_get_val(title_rotY, get_row()), 0.0f, 1.0f, 0.0f);
		font->Printf(ORANGE, 0.4f, Centered, Centered, "&");
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.1f, 0.0f, 0.0f);
		font->Printf(WHITE, 0.15f, Centered, Centered, "NUMBERS");
	glPopMatrix();

	float size = 0.10f;
	float sp = 0.03f;
	float top = 1.0f;
	glPushMatrix();
		glTranslatef(-2.0f, top, 0.0f);

		for (int i = 0; i < 1; i ++)
		{
			font->SetSpacingOnce(-sp, 0.0f);
			font->Printf(WHITE, size, LJustify, LJustify, crossPattern.c_str());

			glTranslatef(size-sp, -size/2.0f, 0.0f);
			font->SetSpacingOnce(-sp, 0.0f);
			font->Printf(ORANGE, size, LJustify, LJustify, crossPattern.c_str());
			glTranslatef(-size+sp, -size/2.0f, 0.0f);

			font->SetSpacingOnce(-sp, 0.0f);
			font->Printf(WHITE, size, LJustify, LJustify, crossPattern.c_str());

			glTranslatef(size-sp, -size/2.0f, 0.0f);
			font->SetSpacingOnce(-sp, 0.0f);
			font->Printf(WHITE, size, LJustify, LJustify, crossPattern.c_str());
			glTranslatef(-size+sp, -size/2.0f, 0.0f);

			font->SetSpacingOnce(-sp, 0.0f);
			font->Printf(ORANGE, size, LJustify, LJustify, crossPattern.c_str());

			glTranslatef(size-sp, -size/2.0f, 0.0f);
			font->SetSpacingOnce(-sp, 0.0f);
			font->Printf(WHITE, size, LJustify, LJustify, crossPattern.c_str());
			glTranslatef(-size+sp, -size/2.0f, 0.0f);
		}

	glPopMatrix();
}
