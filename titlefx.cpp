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
#include "src/sync_data.h"
#endif

void TitleFX::Init(sync_device* rocket)
{
	title_rotY = sync_get_track(rocket, "title_rotY");
}

void TitleFX::Quit()
{
	save_sync(title_rotY, "src/sync_data.h");
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
}
