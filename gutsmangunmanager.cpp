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

GutmanGunManager * GutmanGunManager::instance_ptr = NULL;

GutmanGunManager * GutmanGunManager::instance()
{
  if (GutmanGunManager::instance_ptr == NULL)
  {
    GutmanGunManager::instance_ptr = new GutmanGunManager;
  }

  return GutmanGunManager::instance_ptr;
}

GutmanGunManager::GutmanGunManager()
{
}

GutmanGunManager::~GutmanGunManager()
{
  rock = NULL;
}

void GutmanGunManager::addRock(GutsmanGun * r)
{
  rock = r;
}

void GutmanGunManager::removeRock(GutsmanGun * r)
{
  rock = NULL;
}

void GutmanGunManager::launchRock()
{
  if (rock != NULL) rock->launch();
}
