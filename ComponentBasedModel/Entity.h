#pragma once
#include "TypeAliases.h"
#include "Component.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <bitset>
#include <array>

inline ComponentId GetComponentId()
{
	// last ID
	static ComponentId lastID = 0;
	return ++lastID;
}

template <typename Type>
inline ComponentId GetComponentId()
{
	static ComponentId typeID = GetComponentId();
	return typeID;
}

using ComponentBitSet = std::bitset<32>; // Total : 32
using ComponentArray = std::array<Component*, 32>;

class Entity
{
public:
protected:
	std::vector<Component> m_components;

	ComponentBitSet m_componentBitSet;
	ComponentArray m_componentArray;



	//std::unordered_map<ComponentId, Component*> m_components;
	bool m_isFlipped;
	bool m_isFiring;
	const char* m_pSpritePath;

public:
	virtual ~Entity();

	//template <class Type>
	//Type* GetComponent(ComponentId id);

	template <typename Type>
	Type* GetComponent() const;

	bool IsFlipped() { return m_isFlipped; }
	void SetFlipped(bool flip) { m_isFlipped = flip; }
	bool IsFiring() { return m_isFiring; }
	void Fire(bool fire) { m_isFiring = fire; }

	virtual void OnTriggerEnter(Entity* pOther) = 0;
	virtual void OnCollisionEnter(Entity* pOther) = 0;
protected:
	virtual void Init() = 0;
	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() = 0;

	//bool HasComponent(ComponentId id);
	//void AddComponent(ComponentId id, Component* pComponent);
	//Component* GetComponent (ComponentId id);


	template <typename Type>
	bool HasComponent() const;

	template <typename Type, typename... TArgs>
	Type& AddComponent(TArgs&&... args);

private:

};
template<typename Type>
inline Type* Entity::GetComponent() const
{
	if (HasComponent<Type>())
	{
		auto ptr(m_componentArray[GetComponentId<Type>()]);
		return dynamic_cast<Type*>(ptr);
	}
}

template<typename T>
inline bool Entity::HasComponent() const
{
	return m_componentBitSet[GetComponentId<T>()];
}

template<typename Type, typename... TArgs>
inline Type & Entity::AddComponent(TArgs&&... args)
{
	if (!HasComponent<Type>())
	{
		Type* pComponent(new Type(std::forward<TArgs>(args)...));
		pComponent->m_pEntity = this;
		std::unique_ptr<Component> ptr{ pComponent };
		m_components.emplace_back(std::move(ptr));

		m_componentArray[GetComponentId<Type>()] = pComponent;
		m_componentBitSet[GetComponentId<Type>()] = true;

		pComponent->Init();

		return *pComponent;
	}
}