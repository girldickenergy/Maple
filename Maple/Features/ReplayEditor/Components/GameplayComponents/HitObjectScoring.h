#pragma once

enum HitObjectScoring
{
	/* basic osu! hits */
	Miss,
	Fifty,
	OneHundred,
	ThreeHundred,

	/* internal replay editor states */
	Hit,
	Ignore,
	Notelock
};