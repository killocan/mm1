/*
 * Killocan 2018
 * http://killocan.blogspot.com
*/

#pragma once

#include <vector>
#include <cstddef>

class Stage;
class MagneticBeam;

class MagneticBeamHandler
{
public:
  static MagneticBeamHandler * instance();
  ~MagneticBeamHandler();

  void setup(Stage * stage);

  int addBeam(MagneticBeam * b);
  void removeBeam(MagneticBeam * b);
  void commit();
  void clear();
  bool canCreateAgain();
  int beamsCount();
  void newBeams();

  unsigned long lastInsert;

private:
  MagneticBeamHandler();

  static MagneticBeamHandler * instance_ptr;
  Stage * curStage;
  std::vector<MagneticBeam *> beams;
  bool canShootAgain;
};
