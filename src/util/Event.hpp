#pragma once

#include <set>

namespace util {
	template <typename TListener>
	class Event {
	public:
		void AddListener(TListener* listener);
		void RemoveListener(TListener* listener);

		template <typename ...TArgs>
		void Invoke(void (TListener::* function) (const TArgs&...), const TArgs&... args);

	private:
		std::set<TListener*> m_Listeners;
	};

	template <typename TListener>
	void Event<TListener>::AddListener(TListener* listener) {
		m_Listeners.insert(listener);
	}

	template <typename TListener>
	void Event<TListener>::RemoveListener(TListener* listener) {
		m_Listeners.erase(listener);
	}

	template <typename TListener>
	template <typename ...TArgs>
	void Event<TListener>::Invoke(void (TListener::* function) (const TArgs&...), const TArgs&... args) {
		for (TListener* listener : m_Listeners)
			(listener->*function)(args...);
	}
}