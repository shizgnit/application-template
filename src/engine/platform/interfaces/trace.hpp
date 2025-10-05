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

#define TRACE_SCOPE platform::trace::scoped _scope(trace, __PRETTY_FUNCTION__)

namespace platform {

class trace {
public:

  enum class level {
    SCOPE,
    DEBUG,
    INFO,
    WARNING,
    ERROR
  };

  class scoped {
  public:
    scoped(trace* parent, const std::string& name) : _parent(parent), _name(name) {
        _parent->scope() << "enter >>> " << _name << "";
    }
    ~scoped() {
        _parent->scope() << "exit <<<< " << _name << "";
    }
  protected:
    trace* _parent;
    std::string _name;
  };

  class device {
  public:
    device() {}
    virtual ~device() {}
    virtual void log(level lvl, const std::string& message) = 0;
  };

  class output {
  protected:
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

  inline output operator() (level lvl = level::DEBUG) {
      return { this, lvl };
  }

  inline output scope() {
      return { this, level::SCOPE };
  }

  inline output debug() {
      return { this, level::DEBUG };
  }

  inline output info() {
      return { this, level::INFO };
  }

  inline output warning() {
      return { this, level::WARNING };
  }

  inline output error() {
      return { this, level::ERROR };
  }

  void attach(std::shared_ptr<device> dev) {
    _devices.push_back(dev);
  }

protected:
  std::list<std::shared_ptr<trace::device>> _devices;
};

}