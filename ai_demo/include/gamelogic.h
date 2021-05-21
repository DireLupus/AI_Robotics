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
    int ballColors[3];
  public:

    Tower ()
    {
      ballColors[0] = -1;
      ballColors[1] = -1;
      ballColors[2] = -1;
    }

    Tower (int t, int m, int b)
    {
      ballColors[0] = t;
      ballColors[1] = m;
      ballColors[2] = b;
    }

    int getColor(int index) { return ballColors[index]; }
    int getTop() 
    { 
      if(ballColors[0] != -1)
      { 
        return ballColors[0];
      } else if(ballColors[0] == -1 && ballColors[1] != -1)
      {
        return ballColors[1];
      } else if(ballColors[0] == -1 && ballColors[1] == -1 && ballColors[2] != -1)
      {
        return ballColors[2];
      }

      return -1;
    }
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