/*
 * Killocan 2018
 * http://killocan.blogspot.com
*/

#pragma once

#include <vector>
#include <cstddef>

class Stage;
class GutsmanGun;

class GutmanGunManager
{
public:
  static GutmanGunManager * instance();
  ~GutmanGunManager();

  void addRock(GutsmanGun * b);
  void removeRock(GutsmanGun * b);
  void launchRock();

private:
  GutmanGunManager();

  static GutmanGunManager * instance_ptr;
  GutsmanGun * rock;
};
