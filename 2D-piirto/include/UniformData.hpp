#pragma once
#include <Layout.hpp>
#include <vector>

class UniformData : public Layout{
protected:
  uint liitoskohta {0};
public:
  uint haeLiitoskohta() const;
};
