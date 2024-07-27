#include "creditsfx.h"
#include "palette.h"
#include "crossOpenGL.h"
#include "crossUtil.h"
#include <cmath>

#include "rocket/track.h"

#include "src/direction.hpp"
#ifndef SYNC_PLAYER
    const struct sync_track *credits_names;  // How many names to draw
#else
#include "src/sync_data.h"
#endif

void CreditsFX::Init ( sync_device* rocket )
{
	names.push_back("HANDLE:            ROLE:");
	names.push_back("MUFFINHOP           CODE");
	names.push_back("MUFFINHOP         DESIGN");
	names.push_back("RACCOONVIOLET      MUSIC");
	names.push_back("MUFFINTRAP          CODE");

	credits_names = sync_get_track(rocket, "credits_names");
}

void CreditsFX::Draw ( FontGL* font )
{
	float size = 0.15f;
	glPushMatrix();
	glTranslatef(0.0f - names[0].length() * size / 2.0f, size * names.size(), 0.0f);
	u32 syncAmount = static_cast<u32>(floor(sync_get_val(credits_names, get_row())));
	u32 amount = clampU32(syncAmount, 0, names.size());
	for (size_t ni = 0; ni < amount; ni++)
	{
		font->Printf(WHITE, size, LJustify, LJustify, names[ni].c_str());
		glTranslatef(0.0f, -size, 0.0f);
	}
	glPopMatrix();
}

void CreditsFX::Quit()
{
	save_sync(credits_names, "src/sync_data.h");
}

