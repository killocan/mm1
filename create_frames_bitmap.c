#include <allegro.h>

int main(int argc, char ** argv)
{
	BITMAP * caixas;
	char outfile[80];
	int i;
	int x;
	int w,h,q;

	w = atoi(argv[1]);
	h = atoi(argv[2]);
	q = atoi(argv[3]);
	
	memset(outfile, 80, 0);
	strcpy(outfile, argv[4]);

	allegro_init();
	set_color_depth(24);

	caixas = create_bitmap(w*q+q, h+1);
	clear_to_color(caixas,makecol(255,255,255));

	x = 0;
	putpixel(caixas, 0,0, makecol(255,0,0));
	putpixel(caixas, 1,0, makecol(255,0,0));
	for (i = 0; i < 11; ++i)
	{
		rectfill(caixas, x, 1, x+w-1, h, makecol(255,0,255));
		x += w+1;
		putpixel(caixas, x-1, 0, makecol(0,0,0));
	}

	save_bitmap(outfile, caixas, NULL);

	allegro_exit();

	return 0;
}
