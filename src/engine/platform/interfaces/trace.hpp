/*
================================================================================
  Copyright (c) 2023, Pandemos
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#pragma once

#define trace_scope() trace::scope _scope(trace, __func__)

namespace platform {

class trace {
public:

  enum class level {
    DEBUG,
    INFO,
    WARNING,
    ERROR
  };

  class scope {
  public:
    scope(trace *parent, const std::string &function) : _parent(parent), _function(function) {
      _parent->debug() << "Entering " << _function << "...";
    }
    ~scope() {
      _parent->debug() << "Exiting " << _function << "...";
    }

  protected:
    trace *_parent;
    std::string _function;
  };

  class device {
  public:
    device() {}
    virtual ~device() {}
    virtual void log(level lvl, const std::string& message) = 0;
  };

  class output {
    friend class trace;
    output(trace *parent, level lvl) {
      _parent = parent;
      _level = lvl;
    }
  public:
    ~output() {
      for(auto dev : _parent->_devices) {
          dev->log(_level, _ss.str());
      }
    }

    output& operator << (const std::string& v) {
        _ss << v;
        return *this;
    }
    output& operator << (size_t v) {
        _ss << v;
        return *this;
    }
    output& operator << (int v) {
        _ss << v;
        return *this;
    }
    output& operator << (unsigned int v) {
        _ss << v;
        return *this;
    }
    output& operator << (long v) {
        _ss << v;
        return *this;
    }
    output& operator << (float v) {
        _ss << v;
        return *this;
    }
    output& operator << (double v) {
        _ss << v;
        return *this;
    }

  protected:
    trace *_parent;
    level _level;

    std::stringstream _ss;
  };

  output &operator() (level lvl = level::DEBUG) {
    return output(this);
  }

  void attach(std::shared_ptr<device> dev) {
    _devices.push_back(dev);
  }

protected:
  std::list<std::shared_ptr<trace::device>> _devices;
};

}