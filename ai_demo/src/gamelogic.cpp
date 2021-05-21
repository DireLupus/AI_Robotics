#include "gamelogic.h"

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
