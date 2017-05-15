#include <typeinfo>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace MCNH
{
	struct BadCastException : std::bad_cast{};

	struct Any
	{
		
		struct Placeholder
		{
			virtual void* GetValue() = 0;
			virtual const std::type_info& GetTypeid() = 0;
			virtual ~Placeholder()
			{}
		};

		template<typename T>
		struct ConcretePlaceholder : public Placeholder
		{

			using type = T;

			ConcretePlaceholder(T&& value) : value(std::move(value))
			{}

			void* GetValue() override
			{
				return reinterpret_cast<void*>(&value);		
			}

			const std::type_info& GetTypeid() override
			{
				return typeid(T);
			}

			T value;

		};

		template<typename T>
		Any(T&& value)
		{
			static_assert(std::is_rvalue_reference<decltype(value)>::value, "value is not rvalue reference");
			data = std::unique_ptr<Placeholder>(new ConcretePlaceholder<T>(std::move(value)));
		}

		template<typename T>
		T& operator=(T&& value)
		{
			static_assert(std::is_rvalue_reference<decltype(value)>::value, "value is not rvalue reference");
			data = std::unique_ptr<Placeholder>(new ConcretePlaceholder<T>(std::move(value)));
		}
		
		std::unique_ptr<Placeholder> data = nullptr;

		template<typename T>
		T& To()
		{
			if(typeid(T) != data->GetTypeid())
			{
				throw BadCastException{};
			}
			return *reinterpret_cast<T*>(data->GetValue());
		}
	};
}
