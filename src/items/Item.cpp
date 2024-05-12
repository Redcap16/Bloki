#include <items/Item.hpp>

bool operator==(const Item& lhs, const Item& rhs)
{
	return (lhs.GetType() == rhs.GetType());
}