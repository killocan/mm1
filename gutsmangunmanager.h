/*
 * Killocan 2018
 * http://killocan.blogspot.com
*/

#pragma once

#include <vector>
#include <cstddef>

class Stage;
class GutsmanGun;

class GutsmanGunManager
{
public:
  static GutsmanGunManager * instance();
  ~GutsmanGunManager();

  void addRock(GutsmanGun * b);
  void removeRock(GutsmanGun * b);
  bool launchRock();

private:
  GutsmanGunManager();

  static GutsmanGunManager * instance_ptr;
  GutsmanGun * rock;
};
