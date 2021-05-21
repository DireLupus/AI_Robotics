/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*    Author:       Chris Davis                                               */
/*    Created:      Wed 19 May 2021                                           */
/*                                                                            */
/*    Description:  Core logic for game scoring                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include <queue>
#include <vector>
#include <tuple>
#include <utility>
#include <array>

#define INF 0x3f3f3f3f

using namespace std;

class Tower
{
  private:
    int ballColor;
  public:

    Tower ()
    {
      ballColor = -1;
    }

    Tower (const int& c)
    {
      ballColor = c;
    }

    int getColor() { return ballColor; }
};

class Field
{
  private:
    Tower f[3][3];
    int currentTeam;
    int enemyPoints;
    int friendlyPoints;
  public:
    Field (int c)
    {
      this->currentTeam = c;
      //set default field
      //memset(f,0,sizeof(f)); // This is not the default field.
    }

    void score();
    void update(int,int);
    bool winning();
    int scoreDiff();
};