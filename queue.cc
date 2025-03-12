#include <vector>
#include <cstdio>
#include <algorithm>
#include <string>
#include <cstring>
#include <curses.h>
#include <cstdlib>
#include <deque>
#include <fstream>

using namespace std;

vector<vector<char> > pic;
#if 1
const char * data[] = {
  "******************************************",
  "*                                        *",
  "* ************************************** *",
  "*       *        *             *         *",
  "* * * * ** * * *   * * * * * *   * * * * *",
  "*       *        *             *         *",
  "******** ******** ****** ****** **********",
  "*                                        *",
  "* ************************************** *",
  "*                                        *",
  "* ************************************** *",
  "*                                        *",
  "******************************************",
  0
};
#else
const char * data[] = {
  "******************************************",
  "*                                        *",
  "*   * * * * * * * * * * * * * * * * * *  *",
  "* *                                      *",
  "*   **************************************",
  "* * *                                    *",
  "*   *  * * * * * * * * * * * * * * * * * *",
  "* * *                                    *",
  "*   * * ****************************** * *",
  "* * *                                *   *",
  "*   *  * * * * * * * * * * * * * * * * * *",
  "* * *                                *   *",
  "*   ********************************** * *",
  "* *                                      *",
  "*   * * * * * * * * * * * * * * * * * *  *",
  "*                                        *",
  "******************************************",
  0
};

#endif

typedef pair<int, int> Pos;

Pos operator+(Pos a, Pos b) { return make_pair(a.first+b.first, a.second+b.second); }
Pos operator-(Pos a, Pos b) { return make_pair(a.first-b.first, a.second-b.second); }
Pos operator-(Pos a) { return make_pair(-a.first, -a.second); }
Pos operator+=(Pos & a, Pos b) { return a = make_pair(a.first+b.first, a.second+b.second); }

char & pixel(Pos p) { return pic[p.second][p.first]; }

Pos cur;

void show() {
    for (unsigned i = 0; i < pic.size(); ++i) {
        for (unsigned j = 0; j < pic[i].size(); ++j) {
//            if (Pos(j, i) == cur) mvaddch(i+1, j+1, '@');
//            else if (Pos(j, i) == mark) mvaddch(i+1, j+1, '1');
//            else if (Pos(j, i) == mark2) mvaddch(i+1, j+1, '2');
//            else 
		mvaddch(i+1, j+1, pic[i][j]);
        }
    }
    refresh();
    // usleep(8000);
    // getch();
    napms(2);
}

int fillcnt = 0, stepcnt = 0;

void fill(Pos cur) {
    ++fillcnt;
    pixel(cur) = '*';
}

bool empty(Pos pos) {
    return pixel(pos) != '*';
}

Pos right(Pos dir) { return make_pair(dir.first?0:-dir.second, dir.second?0:dir.first); }
Pos left(Pos dir) { return make_pair(dir.first?0:dir.second, dir.second?0:-dir.first); }

main(int argc, const char ** argv) {
    int startx = 2, starty = 5;
    if (argc == 1) {
        for (const char **p = data; *p; ++p) {
            pic.push_back(vector<char>(*p, *p+strlen(*p)));
        }
        startx = 2;
        starty = 11;
    } else if (argc == 2) {
        if (argv[1][0] != '-' && (argv[1][0] < '0' || argv[1][0] > '9')) {
                std::ifstream in(argv[1]);
                if (!in.good()) {
                        exit(1);
                }
                startx = starty = -1;
                while (!in.eof()) {
                        std::string str;
                        std::getline(in, str);
                        printf("%s\n", str.c_str());
                        pic.push_back(vector<char>(str.begin(), str.end()));
                        if (startx == -1) {
                                startx = str.find('@');
                                if (startx != -1) starty = pic.size() - 1;
                        }
                }
                printf("startx = %d, starty = %d\n", startx, starty);
                getchar();
      } else {
        int SZ = 20;
        sscanf(argv[1], "%d", &SZ);
        if (SZ > 0) {
            for (unsigned i = 0; i < SZ; ++i) {
                pic.push_back(vector<char>(SZ, ' '));
            }
            for (int i = 0; i < SZ; ++i) for (int j = 0; j < SZ; ++j) {
                    if (i == 0 || i == SZ-1 || j == 0 || j == SZ-1) pic[i][j] = '*';
                    int k = i%4, l = j%4;
                    if ((k==l && k < 2) || k*l == 6) pic[i][j] = '*';
                }
        } else {
            SZ = -SZ;
            for (unsigned i = 0; i < SZ; ++i) {
                pic.push_back(vector<char>(SZ, ' '));
            }
            for (int i = 0; i < SZ; ++i) for (int j = 0; j < SZ; ++j) {
                    if (i == 0 || i == SZ-1 || j == 0 || j == SZ-1) pic[i][j] = '*';
                    if (i % 2 == 0 && j % 2 == 0) pic[i][j] = '*';
                }
        }
	startx = starty = SZ/2;
        while (pic[startx][starty] == '*') startx++;
      }
    } else {
        int SZ = 20;
        sscanf(argv[1], "%d", &SZ);
        double ratio = 0.5;
        sscanf(argv[2], "%lf", &ratio);
        if (argc == 5) {
            sscanf(argv[3], "%u", &startx);
            sscanf(argv[4], "%u", &starty);
        }
        for (unsigned i = 0; i < SZ; ++i) {
            pic.push_back(vector<char>(SZ, ' '));
        }
        for (int i = 0; i < SZ; ++i) for (int j = 0; j < SZ; ++j) {
                if (i == 0 || i == SZ-1 || j == 0 || j == SZ-1)
                    pic[i][j] = '*';
                else
                    pic[i][j] = random() < ratio*RAND_MAX ? '*' : ' ';
            } 
        startx = starty = SZ/2;
    }
    initscr();
    curs_set(0);
    cur = make_pair(startx, starty);
    deque<Pos> q;
    q.push_back(cur);
    size_t maxsz = 1;
    while(!q.empty()) {
	++stepcnt;
	cur = q.front(); q.pop_front();
	if (!empty(cur)) continue;
	fill(cur); show();
	if (empty(cur - Pos(1,0))) q.push_front(cur-Pos(1,0));
	if (empty(cur + Pos(1,0))) q.push_front(cur+Pos(1,0));
	if (empty(cur - Pos(0,1))) q.push_back(cur-Pos(0,1));
	if (empty(cur + Pos(0,1))) q.push_back(cur+Pos(0,1));
	maxsz = std::max(maxsz, q.size());
    }
    show();
    getch();
    endwin();
    printf("Steps: %d, fills: %d, max queue: %zd\n", stepcnt, fillcnt, maxsz);
}
