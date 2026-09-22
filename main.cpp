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
	static constexpr double epsilon = 1e-9;

	vector(std::size_t size, const T& value): _size(size), _data(new T[size]) {
		for (std::size_t i = 0; i < size; ++i) {
			_data[i] = value;
		}
	}
	vector(std::size_t size, const T& lower, const T& upper): _size(size), _data(new T[size]) {
		std::random_device numbers;
		std::mt19937 engine(numbers());
		if constexpr (is_complex<T>::value) {
			if (lower.real() > upper.real() || lower.imag() > upper.imag()) {
				throw std::invalid_argument("the lower bound exceeds the upper bound");
			}
		}
		else {
			if (lower > upper) {
				throw std::invalid_argument("the lower bound exceeds the upper bound");
			}
		}
		for (std::size_t i = 0; i < size; ++i) {
			_data[i] = random_value(lower, upper, engine);
		}
	}
	~vector() {
		delete[] _data;
	}
	vector(const vector& other) : _size(other._size) {
		if (other._data) {
			_data = new T[_size];
			for (std::size_t i = 0; i < _size; ++i) {
				_data[i] = other._data[i];
			}
		}
		else {
			_data = nullptr;
			_size = 0;
		}
	}
	vector& operator=(const vector& other) {
		if (this == &other) {
			return *this;
		}
		delete[] _data;
		if (other._data) {
			_size = other._size;
			_data = new T[_size];
			for (std::size_t i = 0; i < _size; ++i) {
				_data[i] = other._data[i];
			}
		}
		else {
			_data = nullptr;
			_size = 0;
		}
		return *this;
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
			throw std::logic_error("the vector dimensions differ");
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
			throw std::logic_error("the vector dimensions differ");
		}
	}
	template<typename S>
	vector operator*(const S& scalar) const {
		vector result(_size, T{});
		for (std::size_t i = 0; i < _size; ++i) {
			result[i] = static_cast<T>(_data[i] * scalar);
		}
		return result;
	}
	template<typename S>
	vector operator/(const S& scalar) const {
		if (scalar == S{}) {
			throw std::invalid_argument("division by zero");
		}
		vector result(_size, T{});
		for (std::size_t i = 0; i < _size; ++i) {
			result[i] = static_cast<T>(_data[i] / scalar);
		}
		return result;
	}
	bool operator==(const vector& other) const {
		if (_size == other._size) {
			for (std::size_t i = 0; i < _size; ++i) {
				if constexpr (is_complex<T>::value) {
					if (std::abs(_data[i] - other._data[i]) > epsilon) {
						return false;
					}
				}
				else if constexpr (std::is_floating_point_v<T>) {
					if (std::abs(_data[i] - other._data[i]) > epsilon) {
						return false;
					}
				}
				else {
					if (_data[i] != other._data[i]) {
						return false;
					}
				}
			}
			return true;
		}
		else {
			return false;
		}
	}
	bool operator!=(const vector& other) const {
		return !(*this == other);
	}
	std::size_t size() const {
		return _size;
	}
	double length() const {
		double sum = 0.0;
		for (std::size_t i = 0; i < _size; ++i) {
			if constexpr (is_complex<T>::value) {
				sum = sum + std::norm(_data[i]);
			}
			else {
				sum = sum + static_cast<double>(_data[i])*static_cast<double>(_data[i]);
			}
		}
		return std::sqrt(sum);
	}
};

template <typename T, typename S>
vector<T> operator*(const S& scalar, const vector<T>& vec) {
	return vec * scalar;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const vector<T>& vec) {
	os << '[';
	for (std::size_t i = 0; i < vec.size(); ++i) {
		if (i != 0) {
			os << "; ";
		}
		os << vec[i];
	}
	os << ']';
	return os;
}

template<typename T>
std::enable_if_t<is_complex<T>::value, vector<T>> task(const vector<T>& a, const vector<T>& b) {
	if (a.size() != b.size()) {
		throw std::logic_error("the vector dimensions differ.");
	}
	double a_len = a.length();
	double b_len = b.length();
	if (a_len == 0.0 || b_len == 0.0) {
		throw std::logic_error("bisector is undefined for zero vector");
	}
	vector<T> a1 = a / a_len;
	vector<T> b1 = b / b_len;
	vector<T> d = a1 + b1;
	if (d.length() == 0.0) {
		throw std::logic_error("bisector is undefined for opposite vectors");
	}
	return d;
}

template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, vector<double>> task(const vector<T>& a, const vector<T>& b) {
	if (a.size() != b.size()) {
		throw std::logic_error("the vector dimensions differ.");
	}
	double a_len = a.length();
	double b_len = b.length();
	if (a_len == 0.0 || b_len == 0.0) {
		throw std::logic_error("bisector is undefined for zero vector");
	}
	vector<double> a1(a.size(), 0.0);
	for (std::size_t i = 0; i < a.size(); ++i) {
		a1[i] = static_cast<double>(a[i]);
	}
	vector<double> b1(b.size(), 0.0);
	for (std::size_t i = 0; i < b.size(); ++i) {
		b1[i] = static_cast<double>(b[i]);
	}
	vector<double> a2 = a1 / a_len;
	vector<double> b2 = b1 / b_len;
	vector<double> d = a2 + b2;
	if (d.length() == 0.0) {
		throw std::logic_error("bisector is undefined for opposite vectors");
	}
	return d;
}

int main() {
	vector<int> a(2, 0);
	a[0] = 3; a[1] = 4;
	std::cout << a;
	vector<int> b(2, 0);
	b[0] = 9; b[1] = 1;
	std::cout << b;
	auto d=task(a, b);
	std::cout << d;
}