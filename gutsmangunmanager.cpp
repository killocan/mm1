/*
 * Killocan 2018
 * http://killocan.blogspot.com
*/

#include "gutsmangunmanager.h"
#include "globals.h"
#include "character.h"
#include "gutsmangun.h"
#include "stage.h"
#include "weapons.h"

GutsmanGunManager * GutsmanGunManager::instance_ptr = NULL;

GutsmanGunManager * GutsmanGunManager::instance()
{
  if (GutsmanGunManager::instance_ptr == NULL)
  {
    GutsmanGunManager::instance_ptr = new GutsmanGunManager;
  }

  return GutsmanGunManager::instance_ptr;
}

GutsmanGunManager::GutsmanGunManager()
{
  rock = NULL;
}

GutsmanGunManager::~GutsmanGunManager()
{
  rock = NULL;
}

void GutsmanGunManager::addRock(GutsmanGun * r)
{
  rock = r;
}

void GutsmanGunManager::removeRock(GutsmanGun * r)
{
  rock = NULL;
}

bool GutsmanGunManager::launchRock()
{
  if (rock != NULL && rock->curState == GutsmanGun::ATTACHED_TO_MEGAMAN)
  {
    rock->launch();

    return true;
  }

  return false;
}
