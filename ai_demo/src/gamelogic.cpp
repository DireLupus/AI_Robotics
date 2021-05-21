#include "gamelogic.h"

void Field::score()
{
  int tempScoreF = 0;
  int tempScoreE = 0;

    for(int x = 0; x < 3; x++)
    {
      for(int y = 0; y < 3; y++)
      {
        for(int i = 0; i < 3; i++)
        {
          if(f[x][y].getColor(i) == currentTeam)
          {
            tempScoreF += 1;
          } else if(f[x][y].getColor(i) != currentTeam && f[x][y].getColor(i) != -1)
          {
            tempScoreE += 1;
          }
        }
      }

      for(int x = 0; x < 3; x++)
      {
        if(f[x][0].getColor(0) == currentTeam && 
           f[x][1].getColor(0) == currentTeam && 
           f[x][2].getColor(0) == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[x][0].getColor(0) != currentTeam && f[x][0].getColor(0) != -1 &&
                  f[x][1].getColor(0) != currentTeam && f[x][1].getColor(0) != -1 &&
                  f[x][2].getColor(0) != currentTeam && f[x][2].getColor(0) != -1 )
        {
          tempScoreE += 6;
        }
      }

      for(int y = 0; y < 3; y++)
      {
        if(f[0][y].getColor(0) == currentTeam && 
           f[1][y].getColor(0) == currentTeam && 
           f[2][y].getColor(0) == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[0][y].getColor(0) != currentTeam && f[0][y].getColor(0) != -1 &&
                  f[1][y].getColor(0) != currentTeam && f[1][y].getColor(0) != -1 &&
                  f[2][y].getColor(0) != currentTeam && f[2][y].getColor(0) != -1 )
        {
          tempScoreE += 6;
        }
      }
    }
        if(f[0][0].getColor(0) == currentTeam && 
           f[1][1].getColor(0) == currentTeam && 
           f[2][2].getColor(0) == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[0][0].getColor(0) != currentTeam && f[0][0].getColor(0) != -1 &&
                  f[1][1].getColor(0) != currentTeam && f[1][1].getColor(0) != -1 &&
                  f[2][2].getColor(0) != currentTeam && f[2][2].getColor(0) != -1 )
        {
          tempScoreE += 6;
        }

        if(f[0][2].getColor(0) == currentTeam && 
           f[1][1].getColor(0) == currentTeam && 
           f[2][0].getColor(0) == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[0][2].getColor(0) != currentTeam && f[0][2].getColor(0) != -1 &&
                  f[1][1].getColor(0) != currentTeam && f[1][1].getColor(0) != -1 &&
                  f[2][0].getColor(0) != currentTeam && f[2][0].getColor(0) != -1 )
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
