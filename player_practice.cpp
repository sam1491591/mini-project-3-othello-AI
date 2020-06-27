#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

struct Point
{
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const
    {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const
    {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const
    {
        return Point(x - rhs.x, y - rhs.y);
    }
};

const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};

std::array<int, 3> disc_count;

int estimate[8][8]=
{
    90,-60,10,10,10,10,-60,90,
    -60,-80,5,5,5,5,-80,-60,
    10,5,1,1,1,1,5,10,
    10,5,1,1,1,1,5,10,
    10,5,1,1,1,1,5,10,
    10,5,1,1,1,1,5,10,
    -60,-80,5,5,5,5,-80,-60,
    90,-60,10,10,10,10,-60,90
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::array<std::array<int, SIZE>, SIZE> second_board;
std::vector<Point> next_valid_spots;
std::vector<Point> second_valid_spots;

void read_board(std::ifstream& fin)
{
    fin >> player;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fin >> board[i][j];
            second_board[i][j]=board[i][j];
            disc_count[board[i][j]]++;
        }
    }
}

void read_valid_spots(std::ifstream& fin)
{
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++)
    {
        fin >> x >> y;
        next_valid_spots.push_back({ x, y });
    }
}

bool is_spot_valid(Point center)
{
    if (second_board[center.x][center.y] != 0)
        return false;
    for (Point dir: directions)
    {
        // Move along the direction while testing.
        Point p = center + dir;
        if (second_board[p.x][p.y]!=3-player)
            continue;
        p = p + dir;
        while ((0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE) && second_board[p.x][p.y]!=0)
        {
            if (second_board[p.x][p.y]==player)
                return true;
            p = p + dir;
        }
    }
    return false;
}

std::vector<Point> get_valid_spots()
{
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            Point p = Point(i, j);
            if (second_board[i][j] != 0)
                continue;
            if (is_spot_valid(p))
                valid_spots.push_back(p);
        }
    }
    return valid_spots;
}

void write_valid_spot(std::ofstream& fout)
{
    //int n_valid_spots = next_valid_spots.size();
    //srand(time(NULL));

    // Choose random spot. (Not random uniform here)
    //int index = (rand() % n_valid_spots);
    //Point p = next_valid_spots.front();
    int p1=disc_count[player]+1,p2=disc_count[3-player];
    int max=-INT_MAX,max2=-INT_MAX;
    int value,value2,state_value=-INT_MAX;
    Point ans;
    for (Point t : next_valid_spots)
    {
        p1=disc_count[player]+1,p2=disc_count[3-player];
        max2=-INT_MAX;
        for (Point dir: directions)
        {
            for(int i=0;i<SIZE;i++)
                for(int j=0;j<SIZE;j++)
                {
                    second_board[i][j]=board[i][j];
                }
            // Move along the direction while testing.
            Point p = t + dir;
            if (board[p.x][p.y]!=3-player)
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while ((0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE) && board[p.x][p.y]!=0)
            {
                if (board[p.x][p.y]==player)
                {
                    for (Point s: discs) {
                        second_board[s.x][s.y]=player;
                    }
                    p1 += discs.size();
                    p2 -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
        value=p1-p2;
        value+=estimate[t.x][t.y];
        if(value>max)
        {
            max=value;
        }
        second_board[t.x][t.y]=player;
        player=3-player;
        second_valid_spots=get_valid_spots();
        player=3-player;
        for (Point t2 : second_valid_spots)
        {
            int second_p1=p1,second_p2=p2+1;
            for (Point dir: directions)
            {
                // Move along the direction while testing.
                Point second_p = t2 + dir;
                if (second_board[second_p.x][second_p.y]!=player)
                    continue;
                std::vector<Point> second_discs({second_p});
                second_p = second_p + dir;
                while ((0 <= second_p.x && second_p.x < SIZE && 0 <= second_p.y && second_p.y < SIZE) && second_board[second_p.x][second_p.y]!=0)
                {
                    if (second_board[second_p.x][second_p.y]==3-player)
                    {
                        second_p1 -= second_discs.size();
                        second_p2 += second_discs.size();
                        break;
                    }
                    second_discs.push_back(second_p);
                    second_p = second_p + dir;
                }
            }
            value2=second_p2-second_p1;
            value2+=estimate[t2.x][t2.y];
            if(value2>max2)
            {
                max2=value2;
            }
        }
        second_board[t.x][t.y]=0;
        if(max-max2>state_value)
        {
            state_value=max-max2;
            ans=t;
        }
    }
    //std::cout<<ans.x<<" "<<ans.y<<std::endl;
    // Remember to flush the output to ensure the last action is written to file.
    fout << ans.x << " " << ans.y << std::endl;
    fout.flush();
}


int main(int, char** argv)
{
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
