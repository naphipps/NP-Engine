//
//  LayerContainer.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/12/21.
//

#ifndef NP_ENGINE_LAYER_CONTAINER_HPP
#define NP_ENGINE_LAYER_CONTAINER_HPP

#include "Layer.hpp"

namespace np
{
    namespace app
    {
        /**
         LayerContainer represents our custom ordering of layers vs overlay, in which overlays are always at the end of the container
         */
        class LayerContainer
        {
        private:
            ui32 _layer_end;
            container::vector<Layer*> _layers;
            using LayerVectorIterator = container::vector<Layer*>::iterator;
            
        public:
            
            /**
             constructor
             */
            LayerContainer():
            _layer_end(0)
            {}
            
            /**
             deconstructor
             */
            ~LayerContainer()
            {
                for (Layer* layer : _layers)
                {
                    if (layer)
                    {
                        layer->Detach();
                    }
                }
            }
            
            /**
             pushes given layer to the end of our layers and before our overlays
             */
            void PushLayer(Layer* layer)
            {
                _layers.emplace(_layers.begin() + _layer_end, layer);
                _layer_end++;
            }
            
            /**
             pushes given overlay to the end of our overlays, which is always after our layers
             */
            void PushOverlay(Layer* overlay)
            {
                _layers.emplace_back(overlay);
            }
            
            /**
             pops the last layer that is right before our first overlay
             */
            Layer* PopLayer()
            {
                Layer* layer = NULL;
                if (_layer_end > 0)
                {
                    _layer_end--;
                    layer = _layers[_layer_end];
                    layer->Detach();
                    _layers.erase(_layers.begin() + _layer_end);
                }
                return layer;
            }
            
            /**
             pops the last overlay
             */
            Layer* PopOverlay()
            {
                Layer* overlay = NULL;
                
                if (_layer_end < _layers.size())
                {
                    overlay = _layers.back();
                    overlay->Detach();
                    _layers.pop_back();
                }
                
                return overlay;
            }
            
            /**
             checks if we contains a layer or overlay
             */
            bl Contains(Layer* layer)
            {
                return ::std::find(_layers.begin(), _layers.end(), layer) != _layers.end();
            }
            
            /**
             removes the given layer or overlay
             */
            bl Remove(const Layer* layer)
            {
                bl removed = false;
                LayerVectorIterator it = ::std::find(_layers.begin(), _layers.end(), layer);
                
                if (it < _layers.end())
                {
                    if (it < _layers.begin() + _layer_end)
                    {
                        _layer_end--;
                    }
                    
                    (*it)->Detach();
                    _layers.erase(it);
                    removed = true;
                }
                
                return removed;
            }
            
            /**
             gets the size of our container
             */
            ui32 Size() const
            {
                return (ui32)_layers.size();
            }
            
            /**
             subscript operator
             */
            Layer* operator[](ui32 index)
            {
                return _layers[index];
            }

            /**
             subscript operator
             */
            const Layer* operator[](ui32 index) const
            {
                return _layers[index];
            }
        };
    }
}

#endif /* NP_ENGINE_LAYER_CONTAINER_HPP */
