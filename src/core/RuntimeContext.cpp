#include <CTRPluginFramework.hpp>
#include "core/RuntimeContext.hpp"

namespace CTRPluginFramework {
    RuntimeContext* RuntimeContext::instance = nullptr;

    RuntimeContext* RuntimeContext::getInstance() {
        if (instance == nullptr) {
            instance = new RuntimeContext();
        }
        return instance;
    }

    bool RuntimeContext::isTurbo() const { 
        return v_isTurbo; 
    }

    void RuntimeContext::setTurbo(bool value) { 
        v_isTurbo = value; 
    }

    bool RuntimeContext::isFov() const { 
        return v_isFov; 
    }

    void RuntimeContext::setFov(bool value) { 
        v_isFov = value; 
    
    }

    bool RuntimeContext::isSaveMenuDisabled() const { 
        return v_isSaveMenuDisabled; 
    
    }

    void RuntimeContext::setSaveMenuDisabled(bool value) { 
        v_isSaveMenuDisabled = value; 
    }

    bool RuntimeContext::isDesignOutfitsLegit() const {
        return v_isDesignOutfitsLegit;
    }

    void RuntimeContext::setDesignOutfitsLegit(bool value) {
        v_isDesignOutfitsLegit = value;
    }
}