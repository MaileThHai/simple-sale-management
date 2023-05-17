#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <functional>
#include <time.h>
#include <stdio.h>

using namespace std;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>> DINH NGHIA CAC STRUCT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< //
    struct Product
{
    int id;
    std::string name;
    unsigned int price;
    int quantity;
    Product *next;
};

struct Invoice
{
    int id;
    time_t time;
    long long total;
    Invoice *next;
    Product *productlist;
};
struct InvoiceListNode
{
    Invoice invoice;
    InvoiceListNode *next;

    InvoiceListNode(const Invoice &_invoice, InvoiceListNode *_next = nullptr) : invoice(_invoice), next(_next){};
};
struct PurchasedItems
{
    int id;
    std::string name;
    int quantity;
    long long cost;
    PurchasedItems *next;
};
struct RevenueByDate 
{
int day;
int month;
long long revenue;
};
struct Revenue
{	
    long long dailyRe;
    long long monthlyRe;
    long long yearlyRe;
    std::vector<RevenueByDate> byDate;
};
struct BiTree
{	Invoice invoice;
    Product data;
    BiTree *left;
    BiTree *right;
};
// ****************************** Bien toan cuc ******************************************* //
InvoiceListNode *invoices_head = nullptr;

int invoice_id_counter = (rand() % 9000) + 1000;
void InvoiceTotal(Invoice &new_invoice, const std::string &filename);
//Product *products_head = nullptr;
//Revenue revenue = {0, 0, 0};

//**************************************************************************************** //
time_t convertDateToTime(int day, int month, int year) {
    std::tm date = {0};
    date.tm_mday = day;
    date.tm_mon = month - 1;
    date.tm_year = year - 1900;

    return std::mktime(&date);
}
time_t stringToTime(const std::string &time_str)
{
    std::tm tm_time = {};
    int year, month, day, hour, minute, second;

    sscanf(time_str.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    tm_time.tm_year = year - 1900;
    tm_time.tm_mon = month - 1;
    tm_time.tm_mday = day;
    tm_time.tm_hour = hour;
    tm_time.tm_min = minute;
    tm_time.tm_sec = second;

    return mktime(&tm_time);
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> CAC HAM DOC/VIET DATA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//=============================== Cap nhat products vao file ============================= //
void writeProductListToFile(Product *head, const std::string &filename)
{
    std::ofstream file(filename);

    if (file.is_open())
    {
        Product *temp = head;
        while (temp != nullptr)
        {
            file << temp->id << ","
                 << temp->name << ","
                 << temp->price << ","
                 << temp->quantity << std::endl;
            temp = temp->next;
        }
    }

    file.close();
}
// ================================ DOC DANH SACH SAN PHAM TU FILE ============================= //
Product *readProductListFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    Product *head = nullptr, *tail = nullptr;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            std::istringstream ss(line);
            std::string field;
            Product *product = new Product;

            getline(ss, field, ',');
            product->id = std::stoi(field);

            getline(ss, field, ',');
            product->name = field;

            getline(ss, field, ',');
            product->price = std::stod(field);

            getline(ss, field, ',');
            product->quantity = std::stoi(field);

            product->next = nullptr;

            if (head == nullptr)
            {
                head = tail = product;
            }
            else
            {
                tail->next = product;
                tail = product;
            }
        }
    }

    file.close();
    return head;
}
// ============================= Xuat hoa don ======================================
void exportInvoice(const Invoice &invoice, const std::string &filename)
{
    std::ofstream file(filename);

    if (file.is_open())
    {
        file << "ID Hoa don: " << invoice.id << std::endl;
        file << "Thoi gian: " << std::asctime(std::localtime(&invoice.time)) << std::endl;
        file << "San pham:" << std::endl;
        Product *temp = invoice.productlist;
        while (temp != nullptr)
        {
            file << temp->name << ", So luong: " << temp->quantity << ", Gia: " << temp->price << std::endl;
            temp = temp->next;
        }

        file << "Tong cong: " << invoice.total << std::endl;
    }

    file.close();
}
// ================================= CAP NHAT LAI DOANH THU ===================================== //
void updateRevenue(Revenue &revenue, long long invoice_total, time_t invoice_time)
{
    time_t now = time(nullptr);
    struct tm *now_tm = localtime(&now);
    struct tm *invoice_tm = localtime(&invoice_time);

    if (now_tm->tm_year == invoice_tm->tm_year)
    {
        revenue.yearlyRe += invoice_total;
        if (now_tm->tm_mon == invoice_tm->tm_mon)
        {
            revenue.monthlyRe += invoice_total;
            if (now_tm->tm_mday == invoice_tm->tm_mday)
            {
                revenue.dailyRe += invoice_total;
            }
        }
    }
}
// ==================================== DOC DU LIEU DOANH THU TU FILE ============================ //
Revenue readRevenueFromFile(const std::string &filename)
{
    Revenue revenue = {0, 0, 0};
    std::ifstream file(filename);
    std::string line;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            std::istringstream ss(line);
            std::string field, value;

            getline(ss, field, ':');
            // Add the line below to skip whitespace after the delimiter.
            ss >> std::ws;
            getline(ss, value, ' ');

            if (field == "Doanh thu theo ngay")
            {
                revenue.dailyRe = std::stod(value);
            }
            else if (field == "Doanh thu theo thang")
            {
                revenue.monthlyRe = std::stod(value);
            }
            else if (field == "Doanh thu theo nam")
            {
                revenue.yearlyRe = std::stod(value);
            }
        }
    }
    file.close();
    return revenue;
}
// ================================= LUU DU LIEU DOANH THU VAO FILE ===================================//
void writeRevenueToFile(const Revenue &revenue, const std::string &filename)
{
    std::ofstream file(filename);

    if (file.is_open())
    {
        file << "Doanh thu theo ngay: " << revenue.dailyRe << std::endl;
        file << "Doanh thu theo thang: " << revenue.monthlyRe << std::endl;
        file << "Doanh thu theo nam: " << revenue.yearlyRe << std::endl;
    }

    file.close();
}

// ================================== THEM SAN PHAM MUA VAO HOA DON =======================================//
void addProductToInvoice(Product *product_list, Invoice &invoice, int product_id, int quantity)
{
    Product *current_product = product_list;

    while (current_product != nullptr)
    {
        if (current_product->id == product_id)
        {
            // Cap nhat lai so luong san pham
            current_product->quantity -= quantity;

            Product *new_product = new Product;
            *new_product = *current_product;
            new_product->quantity = quantity;
            new_product->next = nullptr;

            if (invoice.productlist == nullptr)
            {
                invoice.productlist = new_product;
            }
            else
            {
                Product *temp = invoice.productlist;
                while (temp->next != nullptr)
                {
                    temp = temp->next;
                }
                temp->next = new_product;
            }

            invoice.total += current_product->price * quantity;
            break;
        }
        current_product = current_product->next;
    }
}

// =============================== Xuat hoa don ra file ================================
void createInvoice(Invoice &new_invoice, Product *product_list, Revenue &revenue, PurchasedItems *purchased_items) {
    new_invoice.id = invoice_id_counter +(rand() %9000 +rand() %2000); // Increment the invoice_id_counter
    new_invoice.time = time(nullptr);
    new_invoice.total = 0;
    new_invoice.next = nullptr;
    new_invoice.productlist = nullptr;

    PurchasedItems *current_item = purchased_items;
    while (current_item != nullptr) {
        int product_id = current_item->id;
        int quantity = current_item->quantity;
        addProductToInvoice(product_list, new_invoice, product_id, quantity);
        current_item = current_item->next;
    }

    // Cap nhat doanh thu
    updateRevenue(revenue, new_invoice.total, new_invoice.time);

    // luu vao file
    writeRevenueToFile(revenue, "revenue.txt");

    // Xuat hoa don
    std::string filename = "invoice_" + std::to_string(new_invoice.id) + ".txt";
    exportInvoice(new_invoice, filename);
}
// ======================================== TONG HOP TAT CA HOA DON DA XUAT ============================================ //
void InvoiceTotal(Invoice &new_invoice, const std::string &filename)
{

    // Add the new invoice to the linked list
    InvoiceListNode *new_node = new InvoiceListNode(new_invoice);
    new_node->next = invoices_head;
    invoices_head = new_node;

    // Export the new invoice to a central file
    std::ofstream file(filename, std::ios_base::app);
    if (file.is_open())
    {
        file << "ID Hoa don: " << new_invoice.id << std::endl;

        // Format the invoice time with a more human-readable format
        std::tm *tm_invoice_time = std::localtime(&new_invoice.time);
		char buffer[80];
		strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm_invoice_time);
		file << "Thoi gian: " << buffer << std::endl;


        Product *purchased_product = new_invoice.productlist;
        file << "San Pham: " << std::endl;
        while (purchased_product)
        {
            file << purchased_product->name << ", So luong: " << purchased_product->quantity << ", Gia: " << purchased_product->price << std::endl;
            purchased_product = purchased_product->next;
        }
        file << "Tong cong: " << new_invoice.total << std::endl;
        file << "---" << std::endl;
    }
    file.close();
}
InvoiceListNode *loadInvoicesFromFile() {
    InvoiceListNode *invoices_head = nullptr;
    InvoiceListNode *invoices_tail = nullptr;
	
    std::ifstream file("invoices.txt");
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("ID Hoa don: ") != std::string::npos) {
            Invoice invoice;
            invoice.id = std::stoi(line.substr(12));
            invoice.next = nullptr;
            invoice.productlist = nullptr;

            std::getline(file, line); // Read date
			std::tm tm_invoice_time = {};
			invoice.time = stringToTime(line.substr(11));

            std::getline(file, line); // Read "San Pham:"

            // Read products
            while (true) {
                std::getline(file, line);
                if (line == "---") {
                    break;
                }

                // Process product
                // Expected format: Pepsi, So luong: 2, Gia: 20000
          	    std::stringstream ss(line);
                std::string product_name, tmp;
                std::getline(ss, product_name, ',');
                //product_name.pop_back();  // Bo dau cach cuoi ten san pham

                int quantity;
                std::getline(ss, tmp, ':'); // "So luong"
                ss >> quantity;
                std::getline(ss, tmp, ','); // ", "

                unsigned int price;
                std::getline(ss, tmp, ':'); // "Gia"
                ss >> price;

                Product* new_product = new Product;
                new_product->name = product_name;
                new_product->quantity = quantity;
                new_product->price = price;
                new_product->next = nullptr;

                if (invoice.productlist == nullptr) {
                    invoice.productlist = new_product;
                } else {
                    Product* temp = invoice.productlist;
                    while (temp->next != nullptr) {
                        temp = temp->next;
                    }
                    temp->next = new_product;
                }
            }
            InvoiceListNode *new_node = new InvoiceListNode(invoice);

            if (!invoices_head) {
                invoices_head = invoices_tail = new_node;
            } else {
                invoices_tail->next = new_node;
                invoices_tail = new_node;
            }
        }
    }

    file.close();
    return invoices_head;
}
BiTree *insertInvoiceToTree(BiTree *root, Invoice invoice) {
    if (root == nullptr) {
        root = new BiTree;
        root->invoice = invoice;
        root->left = root->right = nullptr;
    } else if (invoice.id < root->invoice.id) {
        root->left = insertInvoiceToTree(root->left, invoice);
    } else {
        root->right = insertInvoiceToTree(root->right, invoice);
    }
    return root;
}

BiTree *buildInvoiceTree(InvoiceListNode *head) {
    BiTree *root = nullptr;
    while (head != nullptr) {
        root = insertInvoiceToTree(root, head->invoice);
        head = head->next;
    }
    return root;
}

Invoice *searchInvoiceById(BiTree *root, int id) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->invoice.id == id) {
        return &root->invoice;
    } else if (id < root->invoice.id) {
        return searchInvoiceById(root->left, id);
    } else {
        return searchInvoiceById(root->right, id);
    }
}

void displayInvoicesFromTree(BiTree *root) {
    if (root == nullptr) {
        return;
    }

    displayInvoicesFromTree(root->left);

    std::cout << "ID Hoa don: " << root->invoice.id << std::endl;
    std::cout << "Thoi gian: " << std::asctime(std::localtime(&root->invoice.time));
    std::cout << "San Pham: " << std::endl;
    for (Product *p = root->invoice.productlist; p != nullptr; p = p->next) {
        std::cout << p->name << ", So luong: " << p->quantity << ", Gia: " << p->price << std::endl;
    }
    std::cout << "Tong cong: " << root->invoice.total << std::endl;
    std::cout << "---" << std::endl;

    displayInvoicesFromTree(root->right);
}

InvoiceListNode *findInvoicesByTime(InvoiceListNode *head, time_t start_time, time_t end_time)
{
    InvoiceListNode *filtered_invoices_head = nullptr, *filtered_invoices_tail = nullptr;

    while (head)
    {
        if (head->invoice.time >= start_time && head->invoice.time <= end_time)
        {
            InvoiceListNode *new_node = new InvoiceListNode(head->invoice);

            if (!filtered_invoices_head)
            {
                filtered_invoices_head = filtered_invoices_tail = new_node;
            }
            else
            {
                filtered_invoices_tail->next = new_node;
                filtered_invoices_tail = new_node;
            }
        }

        head = head->next;
    }
    return filtered_invoices_head;
}

// ==================================== THEM SAN PHAM ==================================== //
void addProduct(Product *&head, int id, const std::string &name, unsigned int price, int quantity)
{
    Product *newProduct = new Product;
    newProduct->id = id;
    newProduct->name = name;
    newProduct->price = price;
    newProduct->quantity = quantity;
    newProduct->next = nullptr;

    if (head == nullptr)
    {
        head = newProduct;
    }
    else
    {
        Product *last = head;
        while (last->next != nullptr)
        {
            last = last->next;
        }
        last->next = newProduct;
    }
}
// ============================== XOA SAN PHAM ========================================== //
void removeProduct(Product *&head, int id)
{
    if (head == nullptr)
    {
        return;
    }
    if (head->id == id)
    {
        Product *temp = head;
        head = head->next;
        delete temp;
        return;
    }
    Product *current = head;
    while (current->next != nullptr && current->next->id != id)
    {
        current = current->next;
    }
    if (current->next != nullptr)
    {
        Product *temp = current->next;
        current->next = current->next->next;
        delete temp;
    }
}
// ================================= DIEU CHINH SAN PHAM ================================ //
void updateProduct(Product *head, int id, const std::string &name, unsigned int price, int quantity)
{
    Product *current = head;
    while (current != nullptr && current->id != id)
    {
        current = current->next;
    }
    if (current != nullptr)
    {
        current->name = name;
        current->price = price;
        current->quantity = quantity;
    }
}

// ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ Binary Search Tree ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
BiTree *newNode(Product data)
{
    BiTree *temp = new BiTree;
    temp->data = data;
    temp->left = nullptr;
    temp->right = nullptr;
    return temp;
}
BiTree *insert(BiTree *node, Product data)
{
    if (!node)
        return newNode(data);

    if (data.id < node->data.id)
        node->left = insert(node->left, data);
    else
        node->right = insert(node->right, data);

    return node;
}

void inOrderTraversal(BiTree *root, int orderType)
{
    if (root)
    {
        inOrderTraversal(root->left, orderType);

        switch (orderType)
        {
        case 1: // Sort theo ID
            std::cout << "ID: "<<root->data.id
                      << "  Ten:" << std::setw(6) << root->data.name
                      << "  Gia:" << std::setw(10) << root->data.price
                      << "  SoLuong:" << std::setw(3) << root->data.quantity << std::endl;
            break;
        case 2:
            std::cout << "Gia:" << std::setw(10) << root->data.price
                      << "  Ten:" << std::setw(10) << root->data.name
                      << "  ID:" << std::setw(6) << root->data.id
                      << "  SoLuong:" << std::setw(3) << root->data.quantity << std::endl;
            break;
        case 3:
            std::cout << "SoLuong:" << std::setw(5) << root->data.quantity
                      << "  Ten:" << std::setw(10) << root->data.name
                      << "  ID:"<< root->data.id
                      << "  Gia:" << std::setw(10) << root->data.price << std::endl;
            break;
        default:
            break;
        }
        inOrderTraversal(root->right, orderType);
    }
}
BiTree *insertById(BiTree *node, Product data)
{
    if (!node)
        return newNode(data);

    if (data.id < node->data.id)
        node->left = insertById(node->left, data);
    else
        node->right = insertById(node->right, data);

    return node;
}

BiTree *insertByPrice(BiTree *node, Product data)
{
    if (!node)
        return newNode(data);

    if (data.price < node->data.price)
        node->left = insertByPrice(node->left, data);
    else
        node->right = insertByPrice(node->right, data);

    return node;
}

BiTree *insertByQuantity(BiTree *node, Product data)
{
    if (!node)
        return newNode(data);

    if (data.quantity < node->data.quantity)
        node->left = insertByQuantity(node->left, data);
    else
        node->right = insertByQuantity(node->right, data);

    return node;
}
void displayProducts(BiTree *root, int orderType)
{
    std::cout << "\nDanh sach cac san pham:" << std::endl;

    inOrderTraversal(root, orderType);
}

BiTree *populateProductBinaryTree(Product *head, int sortType)
{
    BiTree *root = nullptr;

    while (head != nullptr)
    {
        if (sortType == 1)
            root = insertById(root, *head);
        else if (sortType == 2)
            root = insertByPrice(root, *head);
        else
            root = insertByQuantity(root, *head);

        head = head->next;
    }

    return root;
}
// ==================================== CAC HAM LIEN QUAN DEN QUA TRINH TIM KIEM =================================
Product *findProductById(Product *head, int id)
{
    while (head && head->id != id)
    {
        head = head->next;
    }
    return head;
}
void purchaseProducts(Product *&products_head, Revenue &revenue, PurchasedItems *purchased_items)
{
    Invoice new_invoice;
    createInvoice(new_invoice, products_head, revenue, purchased_items);
    InvoiceTotal(new_invoice, "invoices.txt");
    writeProductListToFile(products_head, "products.txt");
}

void displayRevenue(const Revenue &revenue) {
    std::time_t t = std::time(0); // get the current time
    std::tm* now = std::localtime(&t); // convert the time to local time

    char formattedTime[20];
    std::strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", now); // format the time

    std::cout << "Time: " << formattedTime << std::endl;
    std::cout << "Doanh thu theo ngay: " << revenue.dailyRe << std::endl;
    std::cout << "Doanh thu theo thang: " << revenue.monthlyRe << std::endl;
    std::cout << "Doanh thu theo nam: " << revenue.yearlyRe << std::endl;
}

void displayInvoice(const Invoice &invoice)
{
    std::cout << "ID Hoa Don: " << invoice.id << std::endl;
    std::cout << "Thoi gian: " << std::asctime(std::localtime(&invoice.time)) << std::endl;

    std::cout << "Ten:" << std::endl;
    Product *temp = invoice.productlist;
    while (temp != nullptr)
    {
        std::cout << temp->name << ", SoLuong: " << temp->quantity << ", Gia: " << temp->price << std::endl;
        temp = temp->next;
    }

    std::cout << "Tong cong: " << invoice.total << std::endl;
}

void startPurchasingProcess(Product *&products_head, Revenue &revenue)
{	std::cin.ignore();
    PurchasedItems *purchased_items_head = nullptr, *purchased_items_tail = nullptr;
    std::string input;

    while (true)
    {	
	
        //Nhap ten san pham
        std::cout << "Nhap ten san pham muon mua (nhap 'q' de ket thuc): ";
        
        getline(std::cin, input);

        if (input == "q")
        {
            break;
        }

        std::string product_name = input;

        // Kiem tra san pham
        Product *current_product = products_head;
        while (current_product != nullptr && current_product->name != product_name)
        {
            current_product = current_product->next;
        }

        if (current_product == nullptr)
        {
            std::cout << "Khong tim thay san pham. Vui long kiem tra lai ten nhap." << std::endl;
        }
        else
        {
            //nhap so luong can mua
            int quantity;
            std::cout << "Nhap so luong " << product_name << " can mua: ";
            std::cin >> quantity;
            std::cin.ignore();

            // Them san pham vao danh sach mua
            PurchasedItems *new_item = new PurchasedItems;
            new_item->id = current_product->id;
            new_item->name = current_product->name;
            new_item->quantity = quantity;
            new_item->cost = current_product->price * quantity;
            new_item->next = nullptr;

            if (purchased_items_head == nullptr)
            {
                purchased_items_head = purchased_items_tail = new_item;
            }
            else
            {
                purchased_items_tail->next = new_item;
                purchased_items_tail = new_item;
            }
        }
    }

    // cap nhat lai doanh thu
    if (purchased_items_head != nullptr)
    {
        purchaseProducts(products_head, revenue, purchased_items_head);
    }
}
void traverseInvoiceTree(BiTree *root, int day, int month, int year, long long &dailyRevenue, long long &monthlyRevenue, long long &yearlyRevenue, std::vector<RevenueByDate> &revenueByDate)
{
    if (root != nullptr) {
        std::tm *tm_invoice_time = std::localtime(&root->invoice.time);
        int invoice_day = tm_invoice_time->tm_mday;
        int invoice_month = tm_invoice_time->tm_mon + 1;
        int invoice_year = tm_invoice_time->tm_year + 1900;
        int invoice_hour = tm_invoice_time->tm_hour;
        int invoice_minute = tm_invoice_time->tm_min;
        int startMinute,endMinute,startHour,endHour;

        // Calculate invoice total from products
        long long invoice_total = 0;
        Product *product = root->invoice.productlist;
        while (product != nullptr) {
            invoice_total += product->quantity * product->price;
            product = product->next;
        }

        // Debug output to print the contents of the newly loaded invoice
        std::cout << "Traversing invoice " << root->invoice.id << " - Date: " << invoice_day << "/" << invoice_month << "/" << invoice_year << " - Time: " << invoice_hour << ":" << invoice_minute << " - Total: " << invoice_total << std::endl;

        // Check if the invoice date and time fall within the specified range
        bool isMatch = true;
        if (day != -1 && invoice_day != day) {
            isMatch = false;
        }
        if (month != -1 && invoice_month != month) {
            isMatch = false;
        }
        if (year != -1 && invoice_year != year) {
            isMatch = false;
        }
        std::tm tm_start_time = {0};
        tm_start_time.tm_year = year - 1900;
        tm_start_time.tm_mon = month - 1;
        tm_start_time.tm_mday = day;
        tm_start_time.tm_hour = startHour;
        tm_start_time.tm_min = startMinute;
        std::time_t start_time = std::mktime(&tm_start_time);
        std::tm tm_end_time = {0};
        tm_end_time.tm_year = year - 1900;
        tm_end_time.tm_mon = month - 1;
        tm_end_time.tm_mday = day;
        tm_end_time.tm_hour = endHour;
        tm_end_time.tm_min = endMinute;
        std::time_t end_time = std::mktime(&tm_end_time);
        std::time_t invoice_time = std::mktime(tm_invoice_time);
        if (invoice_time < start_time || invoice_time > end_time) {
            isMatch = false;
        }

        if (isMatch && invoice_total > 0) {
            // Update daily, monthly, and yearly revenue
            dailyRevenue += invoice_total;
            monthlyRevenue += invoice_total;
            yearlyRevenue += invoice_total;

            // Update revenueByDate vector
            bool dateFound = false;
            for (int i = 0; i < revenueByDate.size(); i++) {
                if (revenueByDate[i].day == invoice_day && revenueByDate[i].month == invoice_month) {
                    revenueByDate[i].revenue += invoice_total;
                    dateFound = true;
                    break;
                }
            }
            if (!dateFound) {
                RevenueByDate newRevenueByDate;
                newRevenueByDate.day = invoice_day;
                newRevenueByDate.month = invoice_month;
                newRevenueByDate.revenue = invoice_total;
                revenueByDate.push_back(newRevenueByDate);
            }
        }

        // Traverse left and right subtrees
        traverseInvoiceTree(root->left, day, month, year, dailyRevenue, monthlyRevenue, yearlyRevenue, revenueByDate);
        traverseInvoiceTree(root->right, day, month, year, dailyRevenue, monthlyRevenue, yearlyRevenue, revenueByDate);
    }
     std::cout << "Doanh thu:" << std::endl;
}

void displayMenu()
{
    std::cout << "================| CUA HANG TIEN LOI |====================== " << std::endl;
    std::cout << "=========================================================== " << std::endl;
    std::cout << "|	>> SAU KHI MUA/BAN HAY NHAP SO 0 DE LUU DU LIEU << |" << std::endl;
    std::cout << "|=====================================================     |" << std::endl;
    std::cout << "|	1.  Them san pham                                  |" << std::endl;
    std::cout << "|	2.  Xoa san pham                                   |" << std::endl;
    std::cout << "|	3.  Chinh sua san pham                             |" << std::endl;
    std::cout << "|	4.  Hien thi danh sach san pham                    |" << std::endl;
    std::cout << "|	5.  Mua hang                                       |" << std::endl;
    std::cout << "|	6.  Tim kiem hoa don                               |" << std::endl;
    std::cout << "|	7.  Thong ke doanh thu                             |" << std::endl;
    std::cout << "|	0.  Thoat                                          |" << std::endl;
    std::cout << "=========================================================== " << std::endl;
	std::cout << " Nhap lua chon (0-7): ";
}

void run()
{
    
    while (true)
    {	Product *products_head = readProductListFromFile("products.txt");
    	Revenue revenue = readRevenueFromFile("revenue.txt");
    	InvoiceListNode *invoices_head = loadInvoicesFromFile();
		BiTree *invoice_tree = buildInvoiceTree(invoices_head);
        displayMenu();
        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 0:
            writeProductListToFile(products_head, "products.txt");
            writeRevenueToFile(revenue, "revenue.txt");
            return;
        case 1:
        {
            int id;
            std::string name;
            unsigned int price;
            int quantity;
            std::cout << "Nhap ID, Ten, Gia, Soluong (vd: 10 Pen 10000 100): ";
            std::cin >> id >> name >> price >> quantity;
            addProduct(products_head, id, name, price, quantity);
            writeProductListToFile(products_head, "products.txt");
            break;
        }
        case 2:
        {
            int id;
            std::cout << "Nhap ID san pham muon xoa: ";
            std::cin >> id;
            removeProduct(products_head, id);
            writeProductListToFile(products_head, "products.txt");
            break;
        }
        case 3:
        {
            int id;
            std::string name;
            unsigned int price;
            int quantity;
            std::cout << "Nhap ID, Ten moi, Gia moi, Soluong: ";
            std::cin >> id >> name >> price >> quantity;
            updateProduct(products_head, id, name, price, quantity);
            writeProductListToFile(products_head, "products.txt");
            break;
        }
        case 4:
        {	int k;
			std::cout << "Tuy chon hien thi:" << std::endl;
			std::cout << "1. Sort theo ID " << std::endl;
        	std::cout << "2. Sort theo gia " << std::endl;
        	std::cout << "3. Sort theo soluong " << std::endl;
        	std::cout << "Nhap lua chon: ";
        	std::cin >> k;
			if ( k == 1 )
			{// Sorting by ID
		    std::cout << "Sorting san pham theo ID:" << std::endl;
		    BiTree *rootById = populateProductBinaryTree(products_head, 1);
		    displayProducts(rootById, 1);
			}
			if ( k == 2 )
			{
		    // Sorting by price
		    std::cout << "\nSorting san pham tang dan:" << std::endl;
		    BiTree *rootByPrice = populateProductBinaryTree(products_head, 2);
		    displayProducts(rootByPrice, 2);
			}
			if (k == 3)
			{
		    // Sorting by quantity
		    std::cout << "\nSorting san pham hot sale:" << std::endl;
		    BiTree *rootByQuantity = populateProductBinaryTree(products_head, 3);
		    displayProducts(rootByQuantity, 3);
			}
            break;
        }
        case 5:
        {
            startPurchasingProcess(products_head, revenue);
            break;
        }


        case 6:
		{
                
    			int invoice_id;
			    std::cout << "Nhap ID hoa don can tim: ";
			    std::cin >> invoice_id;
			
			    // Search for the invoice
			    Invoice *found_invoice = searchInvoiceById(invoice_tree, invoice_id);
			    if (found_invoice != nullptr) {
				        std::cout << "Hoa don tim thay:" << std::endl;
				        std::cout << "ID Hoa don: " << found_invoice->id << std::endl;
				        std::cout << "Thoi gian: " << std::asctime(std::localtime(&found_invoice->time));
				        std::cout << "San Pham: " << std::endl;
				        for (Product *p = found_invoice->productlist; p != nullptr; p = p->next) {
				            std::cout << p->name << ", So luong: " << p->quantity << ", Gia: " << p->price << std::endl;
				        }
				        //std::cout << "Tong cong: " << found_invoice->total << std::endl;
				    }
				    else {
				        std::cout << "Khong tim thay hoa don voi ID: " << invoice_id << std::endl;
				    }
            break;
		}



        case 7:
        {
        		displayRevenue(revenue);
        		break;
		}
		case 8:
		{	
			//int day, month, year;
    		//std::cout << "Nhap ngay, thang, nam (dd mm yyyy): ";
    		//	std::cin >> day >> month >> year;
    		
    		long long dailyRevenue = 0;
			long long monthlyRevenue = 0;
			long long yearlyRevenue = 0;
			std::vector<RevenueByDate> revenueByDate;
			traverseInvoiceTree(*root, day, month, year, startHour, startMinute, endHour, endMinute, dailyRevenue, monthlyRevenue, yearlyRevenue, revenueByDate);
			
			// Print out the revenue statistics
			std::cout << "Daily revenue: " << dailyRevenue << std::endl;
			std::cout << "Monthly revenue: " << monthlyRevenue << std::endl;
			std::cout << "Yearly revenue: " << yearlyRevenue << std::endl;
			for (const auto& revenue : revenueByDate) {
			    std::cout << "Revenue on " << revenue.day << "/" << revenue.month << ": " << revenue.revenue << std::endl;
			}
    		
		    break;
		}		
		
			
        default:
		{
            std::cout << "Lua chon khong phu hop, hay chon lai" << std::endl;
            break;
        }
    }
}
}
int main()
{	
	srand(time(NULL));
	run();
    return 0;
}
