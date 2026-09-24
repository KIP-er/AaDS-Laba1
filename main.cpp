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
	
	template<typename E>
	T random_value(const T& lower, const T& upper, E& engine) {
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
	vector(std::size_t size, const T& lower, const T& upper): _size(size) {
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
		_data = (new T[_size]);
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
	if (a_len < vector<T>::epsilon || b_len < vector<T>::epsilon) {
		throw std::logic_error("bisector is undefined for zero vector");
	}
	using value_type = typename T::value_type;
	vector<T> a1 = a / static_cast<value_type>(a_len);
	vector<T> b1 = b / static_cast<value_type>(b_len);
	vector<T> d = a1 + b1;
	if (d.length() < vector<T>::epsilon) {
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
	if (a_len < vector<T>::epsilon || b_len < vector<T>::epsilon) {
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
	if (d.length() < vector<T>::epsilon) {
		throw std::logic_error("bisector is undefined for opposite vectors");
	}
	return d;
}
void print_menu() {
	std::cout << "\n[1] Create a vector: dimension and fill value\n";
	std::cout << "[2] Create a vector: dimension, lower and upper bounds (random)\n";
	std::cout << "[3] Replace an element by index\n";
	std::cout << "[4] Add two vectors\n";
	std::cout << "[5] Subtract two vectors\n";
	std::cout << "[6] Scalar product of two vectors\n";
	std::cout << "[7] Multiply a vector by a scalar\n";
	std::cout << "[8] Divide a vector by a scalar\n";
	std::cout << "[9] Build the bisector of two vectors (task)\n";
	std::cout << "[10] Exit\n";
}

std::size_t read_size(const std::string& prompt) {
	int value = 0;
	while (true) {
		std::cout << prompt;
		std::cin >> value;
		if (value > 0) {
			return static_cast<std::size_t>(value);
		}
		std::cout << "The value must be a positive integer.\n";
	}
}

int read_slot() {
	int slot = 0;
	while (true) {
		std::cout << "Which vector? (1 or 2): ";
		std::cin >> slot;
		if (slot == 1 || slot == 2) {
			return slot - 1;
		}
		std::cout << "Please enter 1 or 2.\n";
	}
}

template <typename T>
T read_value(const std::string& prompt) {
	T value{};
	while (true) {
		std::cout << prompt;
		if constexpr (is_complex<T>::value) {
			std::cout << " (format: (real,imag))";
		}
		std::cout << ": ";
		if (std::cin >> value) {
			return value;
		}
		std::cout << "Invalid input, please try again.\n";
	}
}

template <typename T>
void run_menu() {
	vector<T>* vecs[2] = { nullptr, nullptr };

	while (true) {
		print_menu();
		int choice = 0;
		std::cout << "Enter the menu item: ";
		if (!(std::cin >> choice)) {
			std::cout << "Invalid input, please try again.\n";
			continue;
		}

		try {
			switch (choice) {
			case 1: {
				int slot = read_slot();
				std::size_t size = read_size("Vector dimension: ");
				T value = read_value<T>("Enter the value to fill in");
				delete vecs[slot];
				vecs[slot] = new vector<T>(size, value);
				std::cout << "Vector " << slot + 1 << " = " << *vecs[slot] << '\n';
				break;
			}
			case 2: {
				int slot = read_slot();
				std::size_t size = read_size("Vector dimension: ");
				T lower = read_value<T>("Lower bound: ");
				T upper = read_value<T>("Upper bound: ");
				delete vecs[slot];
				vecs[slot] = new vector<T>(size, lower, upper);
				std::cout << "Vector " << slot + 1 << " = " << *vecs[slot] << '\n';
				break;
			}
			case 3: {
				int slot = read_slot();
				if (!vecs[slot]) {
					std::cout << "Vector " << slot + 1 << " has not been created yet.\n";
					break;
				}
				std::size_t index = read_size("Index: ");
				T value = read_value<T>("New value");
				(*vecs[slot])[index] = value;
				std::cout << "Vector " << slot + 1 << " = " << *vecs[slot] << '\n';
				break;
			}
			case 4: {
				if (!vecs[0] || !vecs[1]) {
					std::cout << "Both vectors must be created first.\n";
					break;
				}
				std::cout << "Sum = " << (*vecs[0] + *vecs[1]) << '\n';
				break;
			}
			case 5: {
				if (!vecs[0] || !vecs[1]) {
					std::cout << "Both vectors must be created first.\n";
					break;
				}
				std::cout << "Difference = " << (*vecs[0] - *vecs[1]) << '\n';
				break;
			}
			case 6: {
				if (!vecs[0] || !vecs[1]) {
					std::cout << "Both vectors must be created first.\n";
					break;
				}
				std::cout << "Scalar product = " << (*vecs[0] * *vecs[1]) << '\n';
				break;
			}
			case 7: {
				int slot = read_slot();
				if (!vecs[slot]) {
					std::cout << "Vector " << slot + 1 << " has not been created yet.\n";
					break;
				}
				T scalar = read_value<T>("Scalar");
				std::cout << "Result = " << (*vecs[slot] * scalar) << '\n';
				break;
			}
			case 8: {
				int slot = read_slot();
				if (!vecs[slot]) {
					std::cout << "Vector " << slot + 1 << " has not been created yet.\n";
					break;
				}
				T scalar = read_value<T>("Scalar");
				std::cout << "Result = " << (*vecs[slot] / scalar) << '\n';
				break;
			}
			case 9: {
				if (!vecs[0] || !vecs[1]) {
					std::cout << "Both vectors must be created first.\n";
					break;
				}
				std::cout << "Bisector = " << task(*vecs[0], *vecs[1]) << '\n';
				break;
			}
			case 10:
				delete vecs[0];
				delete vecs[1];
				std::cout << "Goodbye!\n";
				return;
			default:
				std::cout << "Unknown menu item, please try again.\n";
				break;
			}
		}
		catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << '\n';
		}
	}
}

int main() {
	std::cout << "Select the vector element type:\n";
	std::cout << "  [1] int\n";
	std::cout << "  [2] float\n";
	std::cout << "  [3] double\n";
	std::cout << "  [4] complex<float>\n";
	std::cout << "  [5] complex<double>\n";

	int type = 0;
	while (true) {
		std::cout << "Choice: ";
		std::cin >> type;
		if (type >= 1 && type <= 5) {
			break;
		}
		std::cout << "Invalid choice, please enter a number from 1 to 5.\n";
	}

	switch (type) {
	case 1: 
		run_menu<int>(); 
		break;
	case 2: 
		run_menu<float>(); 
		break;
	case 3: 
		run_menu<double>(); 
		break;
	case 4: 
		run_menu<std::complex<float>>(); 
		break;
	case 5: 
		run_menu<std::complex<double>>(); 
		break;
	}
	return 0;
}