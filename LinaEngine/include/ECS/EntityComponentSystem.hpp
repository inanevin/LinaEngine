/*
Author: Inan Evin - Thanks to the lectures & contributions of Benny Bobaganoosh, thebennybox.
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: EntityComponentSystem
Timestamp: 4/8/2019 5:43:51 PM

*/

#pragma once

#ifndef ECS_HPP
#define ECS_HPP

#include "ECSSystem.hpp"


namespace LinaEngine::ECS
{
	// Listener class for entity actions.
	class ECSListener
	{
	public:

		virtual void OnMakeEntity(EntityHandle handle) {};
		virtual void OnRemoveEntity(EntityHandle handle) {};
		virtual void OnAddComponent(EntityHandle handle, uint32 id) {};
		virtual void OnRemoveComponent(EntityHandle handle, uint32 id) {};

		FORCEINLINE const LinaArray<uint32>& GetComponentIDs() { return componentIDs; }

	protected:

		FORCEINLINE void AddComponentID(uint32 id)
		{
			componentIDs.push_back(id);
		}



	private:

		LinaArray<uint32> componentIDs;
	};


	class EntityComponentSystem
	{
	public:

		EntityComponentSystem() {}
		~EntityComponentSystem();

		FORCEINLINE void AddListener(ECSListener* listener)
		{
			listeners.push_back(listener);
		}
		
		/* Entity Creator */
		EntityHandle MakeEntity(BaseECSComponent** components, const uint32* componentIDs, size_t numComponents);
		
		/* Removes an entity from the sys */
		void RemoveEntity(EntityHandle handle);

		template<class A>
		EntityHandle MakeEntity(A& c1)
		{
			BaseECSComponent* components[] = { &c1 };
			uint32 componentIDs[] = { A::ID };
			return MakeEntity(components, componentIDs, 1);
		}

		template<class A, class B>
		EntityHandle MakeEntity(A& c1, B& c2)
		{
			BaseECSComponent* components[] = { &c1, &c2 };
			uint32 componentIDs[] = { A::ID, B::ID };
			return MakeEntity(components, componentIDs, 2);
		}

		template<class A, class B, class C>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID };
			return MakeEntity(components, componentIDs, 3);
		}

		template<class A, class B, class C, class D>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3, D& c4)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3, &c4 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID };
			return MakeEntity(components, componentIDs, 4);
		}

		template<class A, class B, class C, class D, class E>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3, D& c4, E& c5)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID, E::ID };
			return MakeEntity(components, componentIDs, 5);
		}

		template<class A, class B, class C, class D, class E, class F>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID, E::ID, F::ID };
			return MakeEntity(components, componentIDs, 6);
		}

		template<class A, class B, class C, class D, class E, class F, class G>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID };
			return MakeEntity(components, componentIDs, 7);
		}

		template<class A, class B, class C, class D, class E, class F, class G, class H>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7, H& c8)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID, H::ID };
			return MakeEntity(components, componentIDs, 8);
		}

		template<class A, class B, class C, class D, class E, class F, class G, class H, class I>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7, H& c8, I& c9)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8, &c9 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID, H::ID, I::ID };
			return MakeEntity(components, componentIDs, 9);
		}

		template<class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
		EntityHandle MakeEntity(A& c1, B& c2, C& c3, D& c4, E& c5, F& c6, G& c7, H& c8, I& c9, J& c10)
		{
			BaseECSComponent* components[] = { &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8, &c9, &c10 };
			uint32 componentIDs[] = { A::ID, B::ID, C::ID, D::ID, E::ID, F::ID, G::ID, H::ID, I::ID, J::ID };
			return MakeEntity(components, componentIDs, 10);
		}


		/* Adds component to an entity */
		template<class Component>
		FORCEINLINE void AddComponent(EntityHandle entity, Component* component)
		{
			// Add component.
			AddComponentInternal(entity, HandleToEntity(entity), Component::ID, component);

			// Iterate through the listeners after adding the component.
			for (uint32 i = 0; i < listeners.size(); i++)
			{
				// Get the related component IDs from the listeners.
				const LinaArray<uint32>& componentIDs = listeners[i]->GetComponentIDs();

				// Check if we find the component to be added, if so, call method.
				for (uint32 j = 0; j < componentIDs.size(); j++)
				{
					if (componentIDs[j] == Component::ID)
					{
						listeners[i]->OnAddComponent(entity, Component::ID);
						break;
					}
				}
			}
		}

		/* Removes component from an entity */
		template<class Component>
		bool RemoveComponent(EntityHandle entity)
		{
			// Iterate through the listeners.
			for (uint32 i = 0; i < listeners.size(); i++)
			{
				// Get the related component IDs from the listeners.
				const LinaArray<uint32>& componentIDs = listeners[i]->GetComponentIDs();

				// Check if we find the component to be removed, if so, call method.
				for (uint32 j = 0; j < componentIDs.size(); j++)
				{
					if (componentIDs[j] == Component::ID)
					{
						listeners[i]->OnRemoveComponent(entity, Component::ID);
						break;
					}
				}
			}

			// Remove & return.
			return RemoveComponentInternal(entity, Component::ID);
		}

		/* Gets the typed component out of the entity. */
		template<class Component>
		Component* GetComponent(EntityHandle entity)
		{
			return (Component*)GetComponentInternal(HandleToEntity(entity), components[Component::ID], Component::ID);
		}

		/* System Tick */
		void UpdateSystems(ECSSystemList& systems, float delta);
		

	private:

		// Array of ECSListeners
		LinaArray<ECSListener*> listeners;

		/* Map of id & for each id a seperate array for each comp type */
		LinaMap<uint32, LinaArray<uint8>> components;
		
		/* Entities with pair ids. */
		LinaArray<LinaPair<uint32, LinaArray<LinaPair<uint32, uint32>>>*> entities;

		/* Converts an entity handle to raw type */
		FORCEINLINE LinaPair<uint32, LinaArray<LinaPair<uint32, uint32> > >* HandleToRawType(EntityHandle handle)
		{
			return (LinaPair<uint32, LinaArray<LinaPair<uint32, uint32> > >*)handle;
		}

		/* Pulls the index out of an entity handle. */
		FORCEINLINE uint32 HandleToEntityIndex(EntityHandle handle)
		{
			return HandleToRawType(handle)->first;
		}

		/* Converts an entity handle to entity type */
		FORCEINLINE LinaArray<LinaPair<uint32, uint32> >& HandleToEntity(EntityHandle handle)
		{
			return HandleToRawType(handle)->second;
		}

		void DeleteComponent(uint32 componentID, uint32 index);
		bool RemoveComponentInternal(EntityHandle, uint32 componentID);
		void AddComponentInternal(EntityHandle handle, LinaArray<LinaPair<uint32, uint32>>& entity, uint32 componentID, BaseECSComponent* component);
		BaseECSComponent* GetComponentInternal(LinaArray<LinaPair<uint32, uint32>>& entityComponents, LinaArray<uint8>& arr, uint32 componentID);
		void UpdateSystemMultipleComponentsInternal(uint32 index, ECSSystemList& systems, float delta, const LinaArray<uint32>& componentTypes, LinaArray<BaseECSComponent*>& componentParam, LinaArray<LinaArray<uint8>*>& componentArrays);
		uint32 FindLeastCommonComponent(const LinaArray<uint32>& componentTypes, const LinaArray<uint32>& componentFlags);

		NULL_COPY_AND_ASSIGN(EntityComponentSystem);
	};

}


#endif