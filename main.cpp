import std;

template <typename T>
struct is_complex : std::false_type {};

template <typename U>
struct is_complex<std::complex<U>> : std::true_type{};

template<typename T>
class vector {
private:
	std::size_t _size;
	T* _data;

	template<typename Engine>
	T random_value(const T& lower, const T& upper, Engine& engine) {
		if constexpr (is_complex<T>::value) {
			using value_type = typename T::value_type;
			std::uniform_real_distribution<value_type>real_dist(lower.real(), upper.real());
			std::uniform_real_distribution<value_type>imag_dist(lower.imag(), upper.imag());
			return T(real_dist(engine), imag_dist(engine));
		}
		else if constexpr (std::is_integral_v<T>) {
			std::uniform_int_distribution<T> dist(lower, upper);
			return dist(engine);
		}
		else {
            std::uniform_real_distribution<T> dist(lower, upper);
            return dist(engine);
        }
	}
public:
	vector(std::size_t size, const T& value): _size(size), _data(new T[size]) {
		for (std::size_t i = 0; i < size; ++i) {
			_data[i] = value;
		}
	}
	vector(std::size_t size, const T& lower, const T& upper): _size(size), _data(new T[size]) {
		std::random_device numbers;
		std::mt19937 engine(numbers());
		for (std::size_t i = 0; i < size; ++i) {
			_data[i] = random_value(lower, upper, engine);
		}
	}
	~vector() {
		delete[] _data;
	}
	T& operator[](std::size_t index) {
		if (index >= _size) {
			throw std::out_of_range("index out of range");
		}
		return _data[index];
	}
	const T& operator[](std::size_t index) const{
		if (index >= _size) {
			throw std::out_of_range("index out of range");
		}
		return _data[index];
	}
	vector operator+(const vector& other) const {
		if (_size == other._size) {
			vector result(_size, T{});
			for (std::size_t i = 0; i < _size; ++i) {
				result._data[i] = _data[i] + other._data[i];
			}
			return result;
		}
		else {
			throw std::logic_error("the vector dimensions differ.");
		}
	}
	vector operator-(const vector& other) const {
		if (_size == other._size) {
			vector result(_size, T{});
			for (std::size_t i = 0; i < _size; ++i) {
				result._data[i] = _data[i] - other._data[i];
			}
			return result;
		}
		else {
			throw std::logic_error("the vector dimensions differ.");
		}
	}
	T operator*(const vector& other) const {
		if (_size == other._size) {
			T num = T{};
			if constexpr (is_complex<T>::value) {
				for (std::size_t i = 0; i < _size; ++i) {
					num = num + (std::conj(_data[i]) * other._data[i]);
				}
				return num;
			}
			else {
				for (std::size_t i = 0; i < _size; ++i) {
					num = num + (_data[i] * other._data[i]);
				}
				return num;
			}
		}
		else {
			throw std::logic_error("the vector dimensions differ.");
		}
	}
	vector operator*(const T& scalar) const {
		vector result(_size, T{});
		for (std::size_t i = 0; i < _size; ++i) {
			result[i] = _data[i] * scalar;
		}
		return result;
	}
	vector operator/(const T& scalar) const {
		if (scalar == T{}) {
			throw std::invalid_argument("division by zero");
		}
		vector result(_size, T{});
		for (std::size_t i = 0; i < _size; ++i) {
			result[i] = _data[i] / scalar;
		}
		return result;
	}
};

template <typename T>
vector<T> operator*(const T& scalar, const vector<T>& vec) {
	return vec * scalar;
}
int main() {

}