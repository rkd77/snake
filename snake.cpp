#include <fx.h>
#include <cstdlib>
#include <ctime>

#define SIZE_X 40
#define SIZE_Y 25
#define ROZMIAR (SIZE_X * SIZE_Y)

#define KROK_JABLKA 8
#define KROK_CZACHY 4

#define DEFAULT_TIMEOUT 120000000

unsigned char poleco[ROZMIAR];
unsigned char listax[ROZMIAR];
unsigned char listay[ROZMIAR];
unsigned char last_x, last_y, prev_x, prev_y, cur_x, cur_y, nowy_kierunek;
bool koniec, przerwa, blad, zjadl;
unsigned short int liczba_czach, liczba_jablek, zjedzone, rekord;

unsigned short ile_wolnych;
signed short pocz, kon;
unsigned short liczba_wisienek;
unsigned char stary_kierunek;
bool zjadl_jablko;

enum {
	TLO,
	SCIANA,
	CZACHA,
	JABLKO,
	WISIENKA,
	GLOWA_LEWO,
	GLOWA_GORA,
	GLOWA_PRAWO,
	GLOWA_DOL,
	POZIOM,
	PION,
	LEWO_GORA,
	LEWO_DOL,
	PRAWO_GORA,
	PRAWO_DOL
};

enum {
	LEWO,
	GORA,
	PRAWO,
	DOL
};

short int x_krok[] = {-1, 0, 1, 0};
short int y_krok[] = {0, -1, 0, 1};

FXColor kolor[] = {
	FXRGB(0, 170, 172), // TLO
	FXRGB(172, 170, 172), // SCIANA,
	FXRGB(255, 255, 255), // CZACHA,
	FXRGB(255, 85, 82), // JABLKO,
	FXRGB(255, 85, 255), // WISIENKA,
	FXRGB(82, 255, 82), // GLOWA_LEWO,
	FXRGB(82, 255, 82), // GLOWA_GORA,
	FXRGB(82, 255, 82), // GLOWA_PRAWO,
	FXRGB(82, 255, 82), // GLOWA_DOL,
	FXRGB(0, 170, 0), // POZIOM,
	FXRGB(0, 170, 0), // PION,
	FXRGB(0, 170, 0), // LEWO_GORA,
	FXRGB(0, 170, 0), // LEWO_DOL,
	FXRGB(0, 170, 0), // PRAWO_GORA,
	FXRGB(0, 170, 0), // PRAWO_DOL
};


static int
numer(unsigned char y, unsigned char x)
{
	return y * SIZE_X + x;
}

void
push_front(unsigned char x, unsigned char y)
{
	pocz--;
	if (pocz < 0) pocz = ROZMIAR - 1;
	listax[pocz] = x;
	listay[pocz] = y;
}

void
pop_back(void)
{ /* zwalnia miejsce zajmowane przez ogon */
	kon--;
	if (kon < 0) kon = ROZMIAR - 1;
	unsigned char x = listax[kon];
	unsigned char y = listay[kon];
	int n = numer(y, x);
	poleco[n] = TLO;
	last_x = x;
	last_y = y;
}

void
losuj(unsigned char co)
{
	unsigned short indeks;

	do {
		indeks = (unsigned short)((double)(ROZMIAR) * rand()/(RAND_MAX + 1.0));
	} while (poleco[indeks] != TLO);

	unsigned char x = indeks % SIZE_X;
	unsigned char y = indeks / SIZE_X;

	poleco[indeks] = co;
	ile_wolnych--;
	last_x = x;
	last_y = y;
}


void
losuj_jablka(void)
{
	for (int i = 0; i < liczba_wisienek; i++) losuj(JABLKO);
}

void
losuj_czachy(void)
{
	for (int i = 0; i < liczba_czach; i++) losuj(CZACHA);
}

void
losuj_weza(void)
{
	stary_kierunek = nowy_kierunek = (int)(4.0 * rand()/(RAND_MAX + 1.0));
	unsigned char co = GLOWA_LEWO + nowy_kierunek;
	losuj(co);
	push_front(last_x, last_y);
}

static void
start(unsigned short l_jablek, unsigned short l_czach)
{
	zjedzone = 0;
	for (unsigned int i = KROK_JABLKA; i < l_jablek; i += KROK_JABLKA) {
		zjedzone += i * 2;
	}
	for (int x = 0; x < SIZE_X; x++) {
		poleco[numer(0, x)] = SCIANA;
		poleco[numer(SIZE_Y - 1, x)] = SCIANA;
	}
	for (int y = 1; y < SIZE_Y - 1; y++) {
		poleco[numer(y, 0)] = SCIANA;
		poleco[numer(y, SIZE_X - 1)] = SCIANA;
	}
	signed short i = 0;
	for (unsigned char y = 1; y < SIZE_Y - 1; y++) {
		for (unsigned char x = 1; x < SIZE_X - 1; x++) {
			poleco[numer(y, x)] = TLO;
			i++;
		}
	}
	liczba_wisienek = liczba_jablek = l_jablek;
	liczba_czach = l_czach;
	ile_wolnych = i;
	losuj_jablka();
	losuj_czachy();
	pocz = 0;
	kon = 0;
	losuj_weza();
	koniec = blad = zjadl_jablko = false;
	przerwa = true;
	zjadl = true;
}

static void
start1(void)
{
	start(liczba_jablek, liczba_czach);
}

void
snake(void)
{
	rekord = 0;
	start(8, 4);
};

void
ruch(void)
{
	unsigned char x = listax[pocz];
	unsigned char y = listay[pocz];
	int n = numer(y, x);

	if (zjadl_jablko) {
		if (stary_kierunek == nowy_kierunek) {
			if (stary_kierunek == LEWO || stary_kierunek == PRAWO) poleco[n] = POZIOM;
			else poleco[n] = PION;
		} else {
			if (stary_kierunek == LEWO && nowy_kierunek == GORA) poleco[n] = LEWO_GORA;
			else if (stary_kierunek == LEWO && (nowy_kierunek == DOL || nowy_kierunek == PRAWO)) poleco[n] = LEWO_DOL;
			else if (stary_kierunek == PRAWO && (nowy_kierunek == GORA || nowy_kierunek == LEWO)) poleco[n] = PRAWO_GORA;
			else if (stary_kierunek == PRAWO && nowy_kierunek == DOL) poleco[n] = PRAWO_DOL;
			else if (stary_kierunek == GORA && (nowy_kierunek == LEWO || nowy_kierunek == DOL)) poleco[n] = PRAWO_DOL;
			else if (stary_kierunek == GORA && nowy_kierunek == PRAWO) poleco[n] = LEWO_DOL;
			else if (stary_kierunek == DOL && nowy_kierunek == LEWO) poleco[n] = PRAWO_GORA;
			else if (stary_kierunek == DOL && (nowy_kierunek == PRAWO || nowy_kierunek == GORA)) poleco[n] = LEWO_GORA;
		}
	}

	prev_x = x;
	prev_y = y;
	x += x_krok[nowy_kierunek];
	y += y_krok[nowy_kierunek];
	n = numer(y, x);
	switch (poleco[n]) {
	case TLO:
		pop_back();
		break;
	case JABLKO:
		zjedzone++;
		zjadl = zjadl_jablko = true;
		if (zjedzone > rekord) rekord = zjedzone;
		losuj(WISIENKA);
		break;
	case WISIENKA:
		liczba_wisienek--;
		zjedzone++;
		zjadl = true;
		if (zjedzone > rekord) rekord = zjedzone;
		if (liczba_wisienek == 0) {
			koniec = true;
			przerwa = true;
		} else {
			losuj(CZACHA);
		}
		break;
	default:
		koniec = true;
		przerwa = true;
		blad = true;
		pop_back();
		break;
	}
	poleco[n] = GLOWA_LEWO + nowy_kierunek;
	cur_x = x;
	cur_y = y;
	push_front(x, y);
	stary_kierunek = nowy_kierunek;
}


class SnakeWindow : public FXMainWindow {
	FXDECLARE(SnakeWindow)
private:
	void text();
	void wyswietl();
	FXDCWindow *dc;
protected:
	FXCanvas *canvas;
	FXBitmap *bitmap[PRAWO_DOL + 1];
	FXFont *font;
	SnakeWindow(){}
public:
	SnakeWindow(FXApp* a);
	void create();
	long onPaint(FXObject*, FXSelector, void*);
	long narysuj(FXObject*, FXSelector, void*);
	long klawiatura(FXObject*, FXSelector, void*);
	virtual void save(FXStream& store) const {};
	virtual void load(FXStream& store) {};
	virtual ~SnakeWindow() {};
	enum {
		ID_CANVAS = FXMainWindow::ID_LAST,
		ID_CZAS,
		ID_LEWO,
		ID_GORA,
		ID_PRAWO,
		ID_DOL,
		ID_SPACJA,
		ID_ESCAPE
	};
};


// Map
FXDEFMAP(SnakeWindow) SnakeMap[]={
	FXMAPFUNC(SEL_PAINT, SnakeWindow::ID_CANVAS, SnakeWindow::onPaint),
	FXMAPFUNC(SEL_TIMEOUT, SnakeWindow::ID_CZAS, SnakeWindow::narysuj),
	FXMAPFUNCS(SEL_COMMAND, SnakeWindow::ID_LEWO, SnakeWindow::ID_ESCAPE, SnakeWindow::klawiatura),
};

static FXuchar dane_bits[][32] = {
{ // tlo
 0x80,0x01,0x40,0x02,0x20,0x04,0x10,0x08,
 0x08,0x10,0x04,0x20,0x02,0x40,0x01,0x80,
 0x01,0x80,0x02,0x40,0x04,0x20,0x08,0x10,
 0x10,0x08,0x20,0x04,0x40,0x02,0x80,0x01,
},
{ // sciana
 0xff,0xff,0xdb,0x68,0xaf,0x5f,0xfb,0x37,
 0x77,0x1f,0xff,0x2b,0xaf,0x7c,0xfb,0x1f,
 0xdf,0x2c,0xff,0x7f,0x7f,0x57,0xdb,0x38,
 0x97,0x2b,0x07,0x00,0x03,0x00,0x03,0x00,
},
{ // czacha
 0xf0,0x0f,0xfc,0x3f,0xfe,0x7f,0xff,0xff,
 0xc7,0xe3,0xe7,0xe7,0x7e,0x7e,0x30,0x0c,
 0xe8,0x17,0x08,0x10,0xd0,0x0b,0xf0,0x0f,
 0xe0,0x07,0x80,0x01,0xc0,0x03,0x80,0x01,
},
{ // jablko
 0x10,0x3c,0x30,0x0e,0xc0,0x07,0x00,0x01,
 0x3e,0x3d,0xf3,0x7f,0xf9,0x7f,0xf9,0x7f,
 0xfd,0x7f,0xff,0x7f,0xff,0x3f,0xfe,0x1f,
 0xf8,0x0e,0x00,0x00,0x00,0x00,0x00,0x00,
},
{ // wisienka
 0x00,0x00,0x80,0x0f,0x40,0x18,0x20,0x00,
 0x40,0x00,0xfe,0x79,0xbd,0xf5,0xbd,0xf7,
 0x9f,0xfd,0xee,0x79,0xd0,0x07,0xd0,0x07,
 0xf0,0x07,0xe0,0x03,0x00,0x00,0x00,0x00,
},
{ // glowa w lewo
 0x00,0x00,0xf0,0x1f,0xf8,0x70,0x3c,0xe0,
 0x3a,0xef,0x3d,0xef,0xfd,0xf6,0xff,0xf9,
 0xfd,0xf6,0x3d,0xef,0x3a,0xef,0x3c,0x60,
 0xf8,0x70,0xf0,0x1f,0x00,0x00,0x00,0x00,
},
{ // glowa w gore
 0x00,0x00,0xf0,0x0f,0xc8,0x13,0xf4,0x2f,
 0xfe,0x7f,0xfe,0x7f,0xff,0xff,0xc3,0xc3,
 0x99,0x99,0x39,0x9c,0x39,0x9c,0x99,0x99,
 0xc3,0xc3,0xfe,0x7f,0xfe,0x7f,0xf8,0x1f,
},

{ // glowa w prawo
 0x00,0x00,0xf8,0x07,0x0e,0x0f,0x07,0x1e,
 0xf7,0x2e,0xf7,0x5e,0x6f,0x5f,0x9f,0x7f,
 0x6f,0x5f,0xf7,0x5e,0xf7,0x2e,0x06,0x1e,
 0x0e,0x0f,0xf8,0x07,0x00,0x00,0x00,0x00,
},

{ // glowa w dol
 0xf8,0x1f,0xfe,0x7f,0xfe,0x7f,0xc3,0xc3,
 0x99,0x99,0x39,0x9c,0x39,0x9c,0x99,0x99,
 0xc3,0xc3,0xff,0xff,0xfe,0x7f,0xfe,0x7f,
 0xf4,0x2f,0xc8,0x13,0xf0,0x0f,0x00,0x00,
},

{ // poziom
 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xbd,
 0xff,0xbd,0xff,0x99,0xff,0xc3,0xe7,0xe7,
 0xc3,0xff,0x99,0xff,0xbd,0xff,0xbd,0xff,
 0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
},

{ // pion
0xfc,0x3f,0xfc,0x23,0xfc,0x39,0xfc,0x3c,
0xfc,0x3c,0xfc,0x39,0xfc,0x23,0xfc,0x3f,
0xfc,0x3f,0xc4,0x3f,0x9c,0x3f,0x3c,0x3f,
0x3c,0x3f,0x9c,0x3f,0xc4,0x3f,0xfc,0x3f,
},

{ // lewo gora
0xfc,0x3f,0xfc,0x7f,0xfc,0xff,0x18,0xff,
0x48,0xfe,0xf8,0xfc,0xf0,0xfd,0xf0,0xfd,
0xe0,0xfc,0xc0,0xfc,0x00,0xff,0x00,0xfe,
0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,
},

{ // lewo dol
0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0xfe,
0x00,0xff,0xc0,0xfc,0xe0,0xfc,0xf0,0xfd,
0xf0,0xfd,0xf8,0xfc,0x48,0xfe,0x18,0xff,
0xfc,0xff,0xfc,0x7f,0xfc,0x3f,0xfc,0x3f,
},

{ // prawo gora
0xfc,0x3f,0xfe,0x3f,0xff,0x3f,0xff,0x18,
0x7f,0x12,0x3f,0x1f,0xbf,0x0f,0xbf,0x0f,
0x3f,0x07,0x3f,0x03,0xff,0x00,0x7f,0x00,
0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
},

{ // prawo dol
0x00,0x00,0x00,0x00,0x0f,0x00,0x7f,0x00,
0xff,0x00,0x3f,0x03,0x3f,0x07,0xbf,0x0f,
0xbf,0x0f,0x3f,0x1f,0x7f,0x12,0xff,0x18,
0xff,0x3f,0xfe,0x3f,0xfc,0x3f,0xfc,0x3f,
},
};

// ButtonApp implementation
FXIMPLEMENT(SnakeWindow, FXMainWindow, SnakeMap, ARRAYNUMBER(SnakeMap))

SnakeWindow::SnakeWindow(FXApp *a):FXMainWindow(a,"snake",NULL,NULL, LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP |DECOR_TITLE|DECOR_CLOSE|DECOR_BORDER|DECOR_MENU,100,100,SIZE_X * 16, SIZE_Y * 16)
{
	canvas = new FXCanvas(this, this, ID_CANVAS, LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT , 0, 0, SIZE_X * 16, SIZE_Y * 16);

	snake();
	for (int i = TLO; i <= PRAWO_DOL; i++) bitmap[i] = new FXBitmap(getApp(), dane_bits[i], 0, 16, 16);
	getAccelTable()->addAccel(parseAccel("Left"), this, FXSEL(SEL_COMMAND, ID_LEWO));
	getAccelTable()->addAccel(parseAccel("Up"), this, FXSEL(SEL_COMMAND, ID_GORA));
	getAccelTable()->addAccel(parseAccel("Right"), this, FXSEL(SEL_COMMAND, ID_PRAWO));
	getAccelTable()->addAccel(parseAccel("Down"), this, FXSEL(SEL_COMMAND, ID_DOL));
	getAccelTable()->addAccel(parseAccel("Space"), this, FXSEL(SEL_COMMAND, ID_SPACJA));
	getAccelTable()->addAccel(parseAccel("Esc"), this, FXSEL(SEL_COMMAND, ID_ESCAPE));
	font = new FXFont(getApp(), "8x16");
	//fprintf(stderr, "before addTimeout\n");
	getApp()->addTimeout(this, ID_CZAS, DEFAULT_TIMEOUT);
	//fprintf(stderr, "after addTimeout\n");
}


void
SnakeWindow::text()
{
	if (font) {
		//dc.setFont(font);
		dc->setForeground(FXRGB(172, 170, 172));
		dc->fillRectangle(32, 0, 32, 16);
		dc->fillRectangle(80, 0, 32, 16);
		dc->setBackground(FXRGB(172, 170, 172));
		dc->setForeground(FXRGB(0, 0, 0));
		dc->drawText(32, 12, FXString::value("%04X", zjedzone));
		dc->drawText(80, 12, FXString::value("%04X", rekord));
		dc->setBackground(FXRGB(0, 0, 0));
		zjadl = false;
	}
}

long
SnakeWindow::onPaint(FXObject *obj, FXSelector sel, void *data)
{
	wyswietl();
	return 1;
}

void
SnakeWindow::wyswietl()
{
	dc->setBackground(FXRGB(0, 0, 0));
	for (int i = 0; i < SIZE_Y; i++) {
		for (int j = 0; j < SIZE_X; j++) {
			if (blad && poleco[numer(i, j)] >= GLOWA_LEWO && poleco[numer(i, j)] <= GLOWA_DOL) {
				dc->setForeground(FXRGB(255, 0, 0));
			} else dc->setForeground(kolor[poleco[numer(i, j)]]);
			dc->drawBitmap(bitmap[poleco[numer(i, j)]], j * 16, i * 16);
		}
	}
	text();
}

long
SnakeWindow::narysuj(FXObject *obj, FXSelector sel, void *data)
{
	getApp()->addTimeout(this, FXSEL(SEL_COMMAND, ID_CZAS), DEFAULT_TIMEOUT);
	if (przerwa) return 1;
	ruch();

	dc->setForeground(kolor[poleco[numer(last_y, last_x)]]);
	dc->drawBitmap(bitmap[poleco[numer(last_y, last_x)]], last_x * 16, last_y * 16);
	dc->setForeground(kolor[poleco[numer(prev_y, prev_x)]]);
	dc->drawBitmap(bitmap[poleco[numer(prev_y, prev_x)]], prev_x * 16, prev_y * 16);

	if (blad) {
		dc->setForeground(FXRGB(255, 0, 0));
	} else {
		dc->setForeground(FXRGB(0, 255, 0));
	}
	dc->drawBitmap(bitmap[poleco[numer(cur_y, cur_x)]], cur_x * 16, cur_y * 16);
	if (zjadl) text();

	return 1;
}

long
SnakeWindow::klawiatura(FXObject *obj, FXSelector sel, void *data)
{
	int i = FXSELID(sel);
//fprintf(stderr, "klawiatura: i = %d\n", i);
	switch (i) {
	case ID_ESCAPE:
		getApp()->stop();
		break;
	case ID_SPACJA:
		if (koniec) {
			if (blad) {
				start1();
				wyswietl();
			} else {
				start(liczba_jablek + KROK_JABLKA, liczba_czach + KROK_CZACHY);
				wyswietl();
			}
		} else if (!przerwa) przerwa = true;
		break;
	default:
		nowy_kierunek = i - ID_LEWO;
		if (!koniec) przerwa = false;
		break;
	}
	return 1;
}

void
SnakeWindow::create() {
	FXMainWindow::create();
	for (int i = TLO; i <= PRAWO_DOL; i++) bitmap[i]->create();
	if (font) font->create();
	dc = new FXDCWindow(canvas);
	dc->setFont(font);
	show(PLACEMENT_SCREEN);
}

int
main(int argc, char *argv[])
{
	//FILE *f = fopen("data.dat", "w");
	//fwrite(dane_bits, sizeof(dane_bits), 1, f);
	//fclose(f);
	srand(time(NULL));
	// Make application
	FXApp application("SNAKE","RKD");

	// Open display
	application.init(argc,argv);

	new SnakeWindow(&application);

	// Create app
	application.create();
	return application.run();
}
