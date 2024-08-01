#include "titlefx.h"

#include "crossOpenGL.h"
#include "palette.h"
#include "crossTypedefs.h"
#include "platform.h"


#include "FontGL.h"

#include "rocket/track.h"
#include "rocket/sync.h"
#include "src/direction.hpp"

static ImageGL logo_raccoon;
static ImageGL logo_fcccf;
static ImageGL logo_marmot;

#ifndef SYNC_PLAYER
    const struct sync_track *title_rotY;  // rotation of the symbol
    const struct sync_track *title_patRows;  // how many repetitions of the pattern
    const struct sync_track *title_patX;  // left side of pattern
    const struct sync_track *title_patY;  // top of pattern
    const struct sync_track *title_patScale;  // scale of pattern
    const struct sync_track *title_logos;  // how many logos are shown: 0 show only title
    const struct sync_track *title_logoX;  // left side of logo
    const struct sync_track *title_logoBtwn;  // between logos
#else
	#include "src/sync_data.cpp"
	#include "src/sync_data.h"
#endif

void TitleFX::Init(sync_device* rocket)
{
	crossPattern = "$ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $ $";
#ifndef SYNC_PLAYER
	title_rotY = sync_get_track(rocket, "title:rotY");
	title_patRows = sync_get_track(rocket, "title:patRows");
	title_patX = sync_get_track(rocket, "title:patX");
	title_patY = sync_get_track(rocket, "title:patY");
	title_patScale = sync_get_track(rocket, "title:patScale");
	title_logos = sync_get_track(rocket, "title:logos");
	title_logoX = sync_get_track(rocket, "title:logoX");
	title_logoBtwn = sync_get_track(rocket, "title:logoBtwn");
#endif

	logo_raccoon.LoadImage("apps/numbers_asm24/logo_raccoon.png", GL_LINEAR);
	logo_fcccf.LoadImage("apps/numbers_asm24/logo_fcccf.png", GL_LINEAR);
	logo_marmot.LoadImage("apps/numbers_asm24/logo_marmot.png", GL_LINEAR);
}

void TitleFX::Save()
{
#ifndef SYNC_PLAYER
	save_sync(title_rotY, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(title_patRows, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(title_patX, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(title_patY, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(title_patScale, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(title_logos, SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(title_logoX , SYNC_FILE_H, SYNC_FILE_CPP);
	save_sync(title_logoBtwn, SYNC_FILE_H, SYNC_FILE_CPP);
#endif
}

void TitleFX::DrawLogos(short amount)
{
	float R = get_row();

	glTranslatef(sync_get_val(title_logoX, R), 0.0f, 0.0f);
	glPushMatrix();
		for (int w = 1; w <= amount; w++)
		{
			glTranslatef(sync_get_val(title_logoBtwn, R), 0.0f, 0.0f);
			if (w==1)
			{
				logo_raccoon.Draw3DCentered(1.0f, false);
			}
			else if (w==2)
			{
				logo_marmot.Draw3DCentered(1.0f, false);
			}
			else if (w==3)
			{
				logo_fcccf.Draw3DCentered(1.0f, false);
			}
		}
	glPopMatrix();
}

void TitleFX::DrawPattern(FontGL* font)
{
	float R = get_row();
	float size = 0.10f * sync_get_val(title_patScale, R);
	float sp = 0.03f;
	glPushMatrix();
		glTranslatef(sync_get_val(title_patX, R),
					 sync_get_val(title_patY, R),
					0.0f);

		u32 rows = (u32)floor(sync_get_val(title_patRows, R));
		for (u32 i = 0; i < rows; i ++)
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

void TitleFX::DrawName(FontGL* font)
{
	// Logo and title
	float R = get_row();
	glPushMatrix();
		glTranslatef(-0.7f, 0.0f, 0.0f);
		glRotatef(sync_get_val(title_rotY, R), 0.0f, 1.0f, 0.0f);
		font->Printf(ORANGE, 0.4f, Centered, Centered, "&");
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.1f, 0.0f, 0.0f);
		font->Printf(WHITE, 0.15f, Centered, Centered, "NUMBERS");
	glPopMatrix();
}

void TitleFX::Draw ( FontGL* font )
{
	float R = get_row();

	// What to draw?
	u32 logoAmount = (u32)floor(sync_get_val(title_logos, R));
	if (logoAmount > 0)
	{
		DrawLogos(logoAmount);
	}
	else
	{
		DrawName(font);
	}

	DrawPattern(font);
}
