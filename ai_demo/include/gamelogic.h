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

    void score(int,int);
    void update(int,int);
    bool winning();
    int scoreDiff();
};


void Field::score(int x, int y)
{
  int tempScoreF = 0;
  int tempScoreE = 0;
  for(int i = 0; i < 3; i++)
  {
    for(int c = 0; c < 3; c++)
    {
      if(f[i][c].getColor() == currentTeam)
      {
        tempScoreF += 1;
      }
      if(f[i][c].getColor() != currentTeam && f[i][c].getColor() != -1)
      {
        tempScoreE += 1;
      }
    }
  }
  for(int i = 0; i < 3; i++)
  {
    if( f[i][0].getColor() == currentTeam && 
        f[i][1].getColor() == currentTeam && 
        f[i][2].getColor() == currentTeam )
    {
      tempScoreF += 6;
    } else if( f[i][0].getColor() != currentTeam && f[i][0].getColor() != -1 && 
               f[i][1].getColor() != currentTeam && f[i][1].getColor() != -1 && 
               f[i][2].getColor() != currentTeam && f[i][2].getColor() != -1 )
    {
      tempScoreE += 6;
    } 
  }
  for(int i = 0; i < 3; i++)
  {
    if( f[0][i].getColor() == currentTeam && 
        f[1][i].getColor() == currentTeam && 
        f[2][i].getColor() == currentTeam )
    {
      tempScoreF += 6;
    } else if( f[0][i].getColor() != currentTeam && f[0][i].getColor() != -1 && 
               f[1][i].getColor() != currentTeam && f[1][i].getColor() != -1 && 
               f[2][i].getColor() != currentTeam && f[2][i].getColor() != -1 )
    {
      tempScoreE += 6;
    } 
  }

    if( f[0][0].getColor() == currentTeam && 
        f[1][1].getColor() == currentTeam && 
        f[2][2].getColor() == currentTeam )
    {
      tempScoreF += 6;
    } else if( f[0][0].getColor() != currentTeam && f[0][0].getColor() != -1 && 
               f[1][1].getColor() != currentTeam && f[1][1].getColor() != -1 && 
               f[2][2].getColor() != currentTeam && f[2][2].getColor() != -1 )
    {
      tempScoreE += 6;
    }
    if( f[0][2].getColor() == currentTeam && 
        f[1][1].getColor() == currentTeam && 
        f[2][0].getColor() == currentTeam )
    {
      tempScoreF += 6;
    } else if( f[0][2].getColor() != currentTeam && f[0][2].getColor() != -1 && 
               f[1][1].getColor() != currentTeam && f[1][1].getColor() != -1 && 
               f[2][0].getColor() != currentTeam && f[2][0].getColor() != -1 )
    {
      tempScoreE += 6;
    }  

    friendlyPoints = tempScoreF;
    enemyPoints = tempScoreE;
}

int Field::scoreDiff()
{
  return this->friendlyPoints - this->enemyPoints;
}

bool Field::winning()
{
  if (this->scoreDiff() > 0)
  {
    return true;
  }
  return false;
}
