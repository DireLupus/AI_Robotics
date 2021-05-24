#include "gamelogic.h"


Field::Field(int team) : currentTeam(team) // This registers a constant variable: this is to make it effectively readonly (I was too lazy to make a getter method)
{
  f[0][0] = Tower(-1, 0, 1);
  f[1][0] = Tower(-1, 0, 1);
  f[2][0] = Tower(-1, 0, 1);

  f[0][1] = Tower(1, 0, 1);
  f[1][1] = Tower(-1, -1, -1);
  f[2][1] = Tower(0, 1, 0);

  f[0][2] = Tower(-1, 1, 0);
  f[1][2] = Tower(-1, 1, 0);
  f[2][2] = Tower(-1, 1, 0);
}

Tower Field::get(int x, int y)
{
  return f[x][y]; 
} 
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
        if(f[x][0].getTop() == currentTeam && 
           f[x][1].getTop() == currentTeam && 
           f[x][2].getTop() == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[x][0].getTop() != currentTeam && f[x][0].getTop() != -1 &&
                  f[x][1].getTop() != currentTeam && f[x][1].getTop() != -1 &&
                  f[x][2].getTop() != currentTeam && f[x][2].getTop() != -1 )
        {
          tempScoreE += 6;
        }
      }

      for(int y = 0; y < 3; y++)
      {
        if(f[0][y].getTop() == currentTeam && 
           f[1][y].getTop() == currentTeam && 
           f[2][y].getTop() == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[0][y].getTop() != currentTeam && f[0][y].getTop() != -1 &&
                  f[1][y].getTop() != currentTeam && f[1][y].getTop() != -1 &&
                  f[2][y].getTop() != currentTeam && f[2][y].getTop() != -1 )
        {
          tempScoreE += 6;
        }
      }
    }
        if(f[0][0].getTop() == currentTeam && 
           f[1][1].getTop() == currentTeam && 
           f[2][2].getTop() == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[0][0].getTop() != currentTeam && f[0][0].getTop() != -1 &&
                  f[1][1].getTop() != currentTeam && f[1][1].getTop() != -1 &&
                  f[2][2].getTop() != currentTeam && f[2][2].getTop() != -1 )
        {
          tempScoreE += 6;
        }

        if(f[0][2].getTop() == currentTeam && 
           f[1][1].getTop() == currentTeam && 
           f[2][0].getTop() == currentTeam)
        {
          tempScoreF += 6;
        } else if(f[0][2].getTop() != currentTeam && f[0][2].getTop() != -1 &&
                  f[1][1].getTop() != currentTeam && f[1][1].getTop() != -1 &&
                  f[2][0].getTop() != currentTeam && f[2][0].getTop() != -1 )
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
