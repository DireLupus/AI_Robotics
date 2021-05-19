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
#include <pair>
#include <array>

#define INF 0x3f3f3f3f

using namespace std;

class Field
{
  private:
    Tower f[3][3];
  public:
    Field ()
    {
      //set default field
      //memset(f,0,sizeof(f));
    }

    auto score(int,int)     ->   void;
    auto update(int,int)    ->   void;
    auto winning()          ->   bool;
    auto scoreDiff()        ->   int;
    auto enemyPoints        ->   int;
    auto friendlyPoints     ->   int;
}

class Tower
{
  private:
    int colour;
  public:

    Tower ()
    {
      colour = 0;
    }

    Tower (const int& c)
    {
      colour = c;
    }
}

Field::score(int x, int y)
{
  f[x][y]
}

Field::scoreDiff()
{
  return friendlyPoints() - enemyPoints();
}

Field::winning()
{
  if (scoreDiff()>0)
  {
    return true;
  }
  return false;
}
