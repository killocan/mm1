/*
 * Killocan 2018
 * http://killocan.blogspot.com
*/

#include "magneticbeamhandler.h"

#include "character.h"
#include "magneticbeam.h"
#include "stage.h"

MagneticBeamHandler * MagneticBeamHandler::instance_ptr = NULL;

MagneticBeamHandler * MagneticBeamHandler::instance()
{
  if (MagneticBeamHandler::instance_ptr == NULL)
  {
    MagneticBeamHandler::instance_ptr = new MagneticBeamHandler;
  }

  return MagneticBeamHandler::instance_ptr;
}

MagneticBeamHandler::~MagneticBeamHandler()
{
  beams.clear();
}

void MagneticBeamHandler::setup(Stage * stage)
{
  curStage = stage;
}

int MagneticBeamHandler::addBeam(MagneticBeam * b)
{
  int c = beams.size();
  beams.push_back(b);
  return c+1;
}

void MagneticBeamHandler::removeBeam(MagneticBeam * b)
{
  std::vector<MagneticBeam *>::iterator i;
  for (i = beams.begin(); i != beams.end(); ++i)
  {
    if (*i == b)
    {
      beams.erase(i);
      break;
    }
  }
}

void MagneticBeamHandler::commit()
{
  std::vector<MagneticBeam *>::iterator i;
  for (i = beams.begin(); i != beams.end(); ++i)
  {
    (*i)->commit();
  }
  beams.clear();
}

void MagneticBeamHandler::clear()
{
  beams.clear();
}

bool MagneticBeamHandler::canCreateAgain()
{
  return false;
}

int MagneticBeamHandler::beamsCount()
{
  return beams.size();
}

MagneticBeamHandler::MagneticBeamHandler()
{
}
