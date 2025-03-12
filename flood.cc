#include <vector>
#include <cstdio>
#include <algorithm>
#include <string>
#include <cstring>
#include <curses.h>
#include <cstdlib>
#include <fstream>

using namespace std;

vector<vector<char> > pic;
#if 0
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
  "**********************************************************************************",
  "*                                                                                *",
  "*   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  *",
  "* *                                                                              *",
  "*   ******************************************************************************",
  "* * *                                                                            *",
  "*   *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *",
  "* * *                                                                            *",
  "*   * * ********************************************************************** * *",
  "* * *                                                                        *   *",
  "*   *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *",
  "* * *                                                                        *   *",
  "*   ** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** * *",
  "* *                                                                              *",
  "*   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  *",
  "*                                                                                *",
  "**********************************************************************************",
  0
};

#endif

typedef pair<int, int> Pos;

Pos operator+(Pos a, Pos b) { return make_pair(a.first+b.first, a.second+b.second); }
Pos operator-(Pos a, Pos b) { return make_pair(a.first-b.first, a.second-b.second); }
Pos operator-(Pos a) { return make_pair(-a.first, -a.second); }
Pos operator+=(Pos & a, Pos b) { return a = make_pair(a.first+b.first, a.second+b.second); }

char & pixel(Pos p) { return pic[p.second][p.first]; }

Pos cur, prev, prev2, prev3;
Pos cur_dir;
Pos mark_dir, mark2_dir;
Pos mark;
Pos mark2;

void show() {
    for (unsigned i = 0; i < pic.size(); ++i) {
        for (unsigned j = 0; j < pic[i].size(); ++j) {
            if (Pos(j, i) == cur) mvaddch(i+1, j+1, '@');
            else if (Pos(j, i) == prev) mvaddch(i+1, j+1, 'O');
            else if (Pos(j, i) == prev2) mvaddch(i+1, j+1, 'o');
            else if (Pos(j, i) == prev3) mvaddch(i+1, j+1, '.');
            else if (Pos(j, i) == mark) mvaddch(i+1, j+1, '1');
            else if (Pos(j, i) == mark2) mvaddch(i+1, j+1, '2');
            else mvaddch(i+1, j+1, pic[i][j]);
        }
    }
    refresh();
    // usleep(8000);
    // getch();
    napms(2);
}

int stepcnt = 0, fillcnt = 0;
int stepcnt_at_last_fill = 0;

void fill(Pos cur) {
    ++fillcnt;
    stepcnt_at_last_fill = stepcnt;
    pixel(cur) = '*';
    show();
}

bool backtrack = false;
bool findloop = false;

void step(Pos & cur, Pos dir) {
    ++stepcnt;
    if (stepcnt - stepcnt_at_last_fill > pic.size()*pic.size() * pic.size()) {
	endwin();
	printf("Too many steps without a fill at %d steps\n", stepcnt);
        exit(0);
    } 
    prev3 = prev2;
    prev2 = prev;
    prev = cur;
    cur += dir;
     show();
}

void set(Pos & mark, Pos val, int n) {
    mark = val;
    show();
}

void restore(Pos & mark) {
    if (mark.first >= 100) {
        set(mark, mark - Pos(100,100), -13);
	show();
    }
}

void clear(Pos & mark) {
    if (mark.first < 100) {
        mark += Pos(100, 100);
	show();
    }
}

bool empty(Pos pos) {
    return pixel(pos) != '*';
}

Pos right(Pos dir) { return make_pair(dir.first?0:-dir.second, dir.second?0:dir.first); }
Pos left(Pos dir) { return make_pair(dir.first?0:dir.second, dir.second?0:-dir.first); }

void hatch(int sz) {
    for (unsigned i = 0; i < sz; ++i) {
        pic.push_back(vector<char>(sz, ' '));
    }
    for (int i = 0; i < sz; ++i) for (int j = 0; j < sz; ++j) {
        if (i == 0 || i == sz-1 || j == 0 || j == sz-1) pic[i][j] = '*';
        int k = i%4, l = j%4;
        if ((k==l && k < 2) || k*l == 6) pic[i][j] = '*';
    }
}

void checker(int sz) {
   for (unsigned i = 0; i < sz; ++i) {
       pic.push_back(vector<char>(sz, ' '));
   }
   for (int i = 0; i < sz; ++i) for (int j = 0; j < sz; ++j) {
       if (i == 0 || i == sz-1 || j == 0 || j == sz-1) pic[i][j] = '*';
       if (i % 2 == 0 && j % 2 == 0) pic[i][j] = '*';
   }
}

void randfill(int sz, double ratio) {
    for (unsigned i = 0; i < sz; ++i) {
        pic.push_back(vector<char>(sz, ' '));
    }
    for (int i = 0; i < sz; ++i) for (int j = 0; j < sz; ++j) {
        if (i == 0 || i == sz-1 || j == 0 || j == sz-1)
            pic[i][j] = '*';
        else
            pic[i][j] = random() < ratio*RAND_MAX ? '*' : ' ';
    }        
}

main(int argc, const char ** argv) {
    int startx = -1, starty = -1;
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
	startx = 2; starty = 5;
        sscanf(argv[1], "%d", &SZ);
        if (SZ > 0) {
	    hatch(SZ);
        } else {
            SZ = -SZ;
            checker(SZ);
        }
    }
    } else {
        int SZ = 20;
	startx = 1; starty = 2;
        sscanf(argv[1], "%d", &SZ);
        double ratio = 0.5;
        sscanf(argv[2], "%lf", &ratio);
	if (SZ > 0) {
		if (ratio < 1)
                    randfill(SZ, ratio);
                else
                    hatch(SZ);
	} else {
            SZ = -SZ;
            checker(SZ);
        }
        if (argc == 5) {
            sscanf(argv[3], "%u", &startx);
            sscanf(argv[4], "%u", &starty);
        }
    }

    initscr();
    curs_set(0);
    cur = make_pair(startx, starty);
    cur_dir = make_pair(1, 0);
    mark = make_pair(100, 100);
    mark2 = make_pair(100, 100);

    bool stop = false;
    int count;

    
    while (empty(cur+cur_dir)) {
        step(cur, cur_dir);
    }

    goto START;
    while (!stop) {
        step(cur, cur_dir);
        if (empty(cur+right(cur_dir))) {
            if (backtrack and !findloop and (empty(cur+cur_dir) || empty(cur+left(cur_dir)))) {
                findloop = true;
            }
            cur_dir = right(cur_dir);
          PAINT:
            step(cur, cur_dir);
        }
      START:
        count = 4 -
            empty(cur + cur_dir) -
            empty(cur + left(cur_dir)) -
            empty(cur - cur_dir) -
            empty(cur + right(cur_dir));

        if (count != 4) {
            do {
                cur_dir = right(cur_dir);
            } while (empty(cur + cur_dir));
            do {
                cur_dir = left(cur_dir);
            } while (!empty(cur + cur_dir));
        }
        switch (count) {
        case 1:
            if (backtrack)
                findloop = true;
            else if (findloop) {
                if (mark.first >= 100) {
                    restore(mark);
                }
            } else if (empty(cur + cur_dir + left(cur_dir)) &&
                       empty(cur - cur_dir + left(cur_dir))) {
                clear(mark);
                fill(cur);
                goto PAINT;
            }
            break;
        case 2:
            if (!empty(cur - cur_dir)) {
                if (empty(cur + cur_dir + left(cur_dir))) {
                    clear(mark);
                    fill(cur);
                    goto PAINT;
                }
            } else if (mark.first >= 100) {
                set(mark, cur, 1);
                mark_dir = cur_dir;
                clear(mark2);
                findloop = backtrack = false;
            } else if (mark2.first >= 100) {
                if (cur == mark) {
                    if (cur_dir == mark_dir) {                       
                        clear(mark);
                        cur_dir = -cur_dir;
                        fill(cur);
                        goto PAINT;
                    } else {
                        backtrack = true;
                        findloop = false;
                        cur_dir = mark_dir;
                    }
                } else if (findloop) {
                    set(mark2, cur, 2);
                    mark2_dir = cur_dir;
                }
            } else if (cur == mark) {
                step(cur, mark2 + -cur); // cur = mark2;
                cur_dir = mark2_dir;
                clear(mark);
                clear(mark2);
                backtrack = false;
                cur_dir = -cur_dir;
                fill(cur);
                goto PAINT;
            } else if (cur == mark2) {
                set(mark, cur, 1);
                cur_dir = mark_dir = mark2_dir;
                clear(mark2);
            }
            break;
        case 3:
            clear(mark);
            fill(cur);
            goto PAINT;
            break;
        case 4:
            fill(cur);
            stop = true;
        }
    }
    show();
    getch();
    endwin();
    printf("Steps: %d, fills: %d, efficiency %.2f%%\n", stepcnt, fillcnt, fillcnt*100.0/stepcnt);
}
