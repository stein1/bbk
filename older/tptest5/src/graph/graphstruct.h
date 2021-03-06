#ifndef _GRAPHSTRUCT_H_
#define _GRAPHSTRUCT_H_

#include "valuestruct.h"

/* Bring in gd library functions */
#include "gd.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"

enum fontsizes { gs_none=0, gs_tiny=1, gs_small=2, gs_mediumbold=3, gs_large=4, gs_giant=5 };

class Graphstruct {
public:
	Graphstruct();
	~Graphstruct();
	void add_value(int timestamp, double value);
	void clear_values();
	int load_values(char *filename);
	int save_values(char *filename);
	int generate();
	char * format_timestamp(char *fmtstr, int timestamp);

	// old graphstruct member variables

	int xsize, ysize;  			/* image size */
	int bgcolor;				/* background color */

	char *header;				/* header text */
	int header_offx, header_offy;		/* header offset */
	enum fontsizes header_size;		/* header font */
	int header_color;			/* header text color */

	int plot_bgcolor;				/* plot background color */
	int dash_color;					/* color of dashed lines */

	char *x_units;			/* x units text */
	enum fontsizes x_units_size;		/* x units font */
	int x_units_offx, x_units_offy;	/* x units text offset */
	int x_units_color;			/* x units text color */

	int x_axis_y_offset;			/* x axis y offset in image */
	int x_axis_left, x_axis_right;	/* x axis x-coordinates in image */
	int x_start, x_stop, x_step;		/* x axis marks */
	int x_mark_height;			/* height in pixels of x axis marks */
	int x_color;				/* x axis color */
	enum fontsizes x_values_size;		/* x values (datetimes) */
	char x_step_str[20];			/* datetime display format */
	char x_step_str2[20];			/* line 2 */

	char y_units[20];			/* y units text */
	enum fontsizes y_units_size;		/* y units font */
	int y_units_offx, y_units_offy;	/* y units text offset */
	int y_units_color;			/* y units text color */

	int y_axis_x_offset;			/* y axis x offset in image */
	int y_axis_top, y_axis_bottom;	/* y axis y-coordinates in image */
	double y_start, y_stop, y_step;	/* y axis marks */
	double y_divisor;				/* Divide results by this number */
	enum fontsizes y_value_size;		/* y values */
	int y_value_decimals;			/* decimal figures used for y values */
	int y_value_x_offset;			/* x offset for y values */
	int y_mark_length;			/* lenght in pixels of y axis marks */
	int y_color;				/* y axis color */

	int info_x_offset;			/* x offset of info box */
	int info_y_offset;			/* y offset of info box */
	int info_bgcolor;			/* background color of info box */
	int info_box_maxcolor;		/* max/min/avg text colors */
	int info_box_avgcolor;
	int info_box_mincolor;
	int info_legend;			/* if nonzero, do legend instead of box */
	int adash_color;			/* average line dash color */
	enum fontsizes info_box_font;		/* info box font size */

	int point_color;			/* color of graph points */
	int point_width;			/* height of graph points */
	int point_height;			/* width of graph points */

	/* These are internal variables that should not be altered */
	int info_xsize;		/* info box size */
	int info_ysize;		/* info box size */
	struct valuestruct *values;
	int no_values;		/* do not change */
	gdFontPtr font_arr[6];	/* do not change */
	gdImagePtr image;		/* only read, do not alter */


};

// Default values

#define XSIZE			300
#define YSIZE			200
#define X_AXIS_LEFT		30
#define X_AXIS_RIGHT		270
#define Y_AXIS_TOP		30
#define Y_AXIS_BOTTOM		170

#define Y_DIVISOR		1

#define INFO_SIZE_X		60
#define INFO_SIZE_Y		40

#define X_MARK_HEIGHT		6
#define Y_MARK_LENGTH		6
#define POINT_WIDTH		5
#define POINT_HEIGHT		5

#define BGCOLOR			(0xC0C0C0)
#define PLOT_BGCOLOR	(0xFFFFFF)
#define DASH_COLOR		(0xC0C0C0)
#define ADASH_COLOR		(0x00BB00)
#define X_COLOR		(0x000000)
#define Y_COLOR		(0x000000)
#define X_UNITS_COLOR	(0x000000)
#define Y_UNITS_COLOR	(0x000000)
#define INFO_BGCOLOR	(0xF0F0F0)
#define INFO_MAXCOLOR	(0xFF0000)
#define INFO_MINCOLOR	(0x0000FF)
#define INFO_AVGCOLOR	(0x00BB00)
#define HEADER_COLOR	(0x000000)
#define POINT_COLOR	(0xFF5050)

#define Y_UNITS_SIZE	gs_small
#define X_UNITS_SIZE	gs_small
#define Y_VALUE_SIZE	gs_tiny
#define X_VALUE_SIZE	gs_tiny
#define INFO_BOX_FONT	gs_small



#define GDCOLORALLOC(A,B) gdImageColorAllocate(A,(B&0xFF0000)>>16,(B&0xFF00)>>8,B&0xFF)


#endif
