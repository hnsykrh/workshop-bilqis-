# Project Constraints and Limitations
## Dress Rental Management System

---

## **1. DATABASE & INFRASTRUCTURE CONSTRAINTS**

### **Current Limitations:**
- **Hardcoded Database Credentials:** Database connection settings (host, user, password) are hardcoded in `DatabaseManager.cpp`
  - Host: `localhost:3306` (fixed)
  - User: `root` (hardcoded)
  - Password: Empty (hardcoded)
  - Database: `dress_rental_db` (fixed)
- **Single Database Connection:** No connection pooling, only one connection at a time
- **No Connection String Configuration:** Cannot change database without recompiling
- **No Database Migration Support:** Manual SQL script execution required
- **Limited Indexing:** Basic indexes only, no composite indexes for complex queries
- **No Database Backup/Recovery:** No automated backup functionality

### **Future Improvements:**
1. **Configuration File System:**
   - Use JSON/XML/INI file for database configuration
   - Read credentials from environment variables
   - Support multiple database configurations (dev, staging, production)
   - Implement configuration validation

2. **Connection Pooling:**
   - Implement connection pool manager
   - Support multiple concurrent connections
   - Automatic connection retry on failure
   - Connection timeout and health checks

3. **Database Migration System:**
   - Automated migration scripts
   - Version control for database schema
   - Rollback capabilities
   - Schema validation on startup

4. **Enhanced Indexing:**
   - Composite indexes for frequently queried columns
   - Full-text search indexes for dress names
   - Query performance optimization
   - Index usage monitoring

5. **Backup & Recovery:**
   - Automated daily/weekly backups
   - Point-in-time recovery
   - Export/import functionality
   - Database snapshot capabilities

---

## **2. SECURITY CONSTRAINTS**

### **Current Limitations:**
- **Hardcoded Password Salt:** Fixed salt string `"dress_rental_salt_2025_"` in password hashing
- **Simple Hashing Algorithm:** SHA-256 (should use bcrypt/Argon2 for better security)
- **No Password Policy Enforcement:** New sign-ups have password requirements, but existing users don't
- **No Session Management:** No session timeout, no session tokens
- **No Password Encryption in Transit:** Passwords sent in plain text over connection (though local)
- **No Audit Trail:** ActivityLog table exists but not fully implemented
- **Weak Authentication:** Only username/password, no 2FA
- **No Role-Based Access Control Granularity:** Only Administrator/Staff roles, no fine-grained permissions

### **Future Improvements:**
1. **Enhanced Password Security:**
   - Use bcrypt or Argon2 instead of SHA-256
   - Unique salt per password
   - Password strength meter
   - Password expiration policy
   - Force password change on first login

2. **Session Management:**
   - Implement session tokens
   - Session timeout (e.g., 30 minutes inactivity)
   - Concurrent session limits
   - Session tracking and management

3. **Multi-Factor Authentication (2FA):**
   - SMS/Email OTP verification
   - TOTP (Time-based One-Time Password)
   - Biometric authentication support

4. **Enhanced Access Control:**
   - Fine-grained permissions (create, read, update, delete per module)
   - Permission groups/roles
   - IP whitelisting
   - Login attempt rate limiting

5. **Security Features:**
   - Encrypted database connections (SSL/TLS)
   - Data encryption at rest
   - Secure password reset flow
   - Account lockout after failed attempts
   - Security event logging

---

## **3. BUSINESS RULES CONSTRAINTS**

### **Current Limitations:**
- **Hardcoded Business Rules:** All business rules are hardcoded in the code
  - Age limit: 18 years (hardcoded)
  - Max rentals: 3 per customer (hardcoded)
  - Max dresses: 5 per rental (hardcoded)
  - Rental duration: 1-14 days (hardcoded)
  - Late fee: RM 10 per day (hardcoded)
- **No Configurable Rules:** Cannot change business rules without code modification
- **No Business Rule Versioning:** Cannot track rule changes over time
- **No Rule Validation at Database Level:** Only application-level validation
- **Limited Validation:** Basic validation only, no complex business logic validation

### **Future Improvements:**
1. **Configurable Business Rules System:**
   - Store business rules in database table
   - Admin interface to modify rules
   - Rule validation and testing
   - Rule versioning and audit trail

2. **Enhanced Validation:**
   - Database-level constraints (CHECK constraints)
   - Stored procedures for complex validations
   - Business rule engine
   - Validation rules API

3. **Flexible Business Rules:**
   - Different rules for different customer tiers
   - Seasonal pricing rules
   - Discount and promotion rules
   - Late fee calculation variations (percentage, fixed, graduated)

4. **Business Rule Testing:**
   - Unit tests for business rules
   - Rule simulation and testing
   - Impact analysis before rule changes

---

## **4. PERFORMANCE CONSTRAINTS**

### **Current Limitations:**
- **No Caching:** Every query hits the database directly
- **No Pagination:** All records loaded at once (can be slow with large datasets)
- **Fixed Report Limits:** Reports limited to "Top 20" or "LIMIT 20" (hardcoded)
- **No Query Optimization:** No query result caching
- **Single-Threaded:** No multi-threading support
- **No Lazy Loading:** All related data loaded immediately
- **Large Result Sets:** Loading all customers/dresses/payments at once
- **No Database Query Batching:** Individual queries for each operation

### **Future Improvements:**
1. **Caching System:**
   - Redis/Memcached integration
   - Cache frequently accessed data (dress inventory, customer list)
   - Cache invalidation strategies
   - Query result caching

2. **Pagination:**
   - Implement pagination for all list views
   - Configurable page size
   - Virtual scrolling for large datasets
   - "Load more" functionality

3. **Query Optimization:**
   - Query result caching
   - Prepared statement pooling
   - Query execution plan analysis
   - Database query optimization

4. **Performance Monitoring:**
   - Query execution time tracking
   - Slow query logging
   - Performance metrics dashboard
   - Database performance monitoring

5. **Asynchronous Operations:**
   - Background task processing
   - Async report generation
   - Non-blocking database operations

---

## **5. USER INTERFACE CONSTRAINTS**

### **Current Limitations:**
- **Text-Based Interface Only:** Command-line interface, no GUI
- **Fixed Screen Width:** `SCREEN_WIDTH = 80` (hardcoded, cannot resize)
- **Limited Data Visualization:** ASCII charts only, no graphical charts
- **No Export Functionality:** Cannot export reports to CSV/PDF/Excel
- **Limited Print Support:** No formatted printing
- **No Search Filtering:** Basic search only, no advanced filtering
- **No Sorting Options:** Fixed sorting order
- **Limited Color Support:** Basic ANSI colors, may not work on all terminals

### **Future Improvements:**
1. **Web-Based GUI:**
   - HTML5/CSS3/JavaScript web interface
   - Responsive design (mobile-friendly)
   - Modern UI framework (React, Vue, Angular)
   - Real-time updates

2. **Desktop GUI:**
   - Qt or GTK+ desktop application
   - Native look and feel
   - Drag-and-drop functionality
   - Rich text formatting

3. **Enhanced Visualization:**
   - Interactive charts (Chart.js, D3.js)
   - Graphs and visual analytics
   - Dashboard widgets
   - Real-time data visualization

4. **Export Functionality:**
   - PDF export with formatting
   - Excel/CSV export
   - Email report sending
   - Scheduled report generation

5. **Advanced Search & Filter:**
   - Multi-criteria filtering
   - Advanced search with operators
   - Saved search filters
   - Quick filters and tags

6. **Customization:**
   - User preferences (theme, layout)
   - Customizable dashboard
   - Column visibility toggles
   - Sortable columns

---

## **6. SCALABILITY CONSTRAINTS**

### **Current Limitations:**
- **Single-Threaded Application:** Cannot handle concurrent users
- **No Multi-User Support:** Designed for single user at a time
- **Memory Management:** Manual memory management with `new`/`delete`
- **No Distributed System Support:** Cannot scale horizontally
- **No Load Balancing:** Single instance only
- **Limited Concurrency:** Database transactions may block each other
- **No Horizontal Scaling:** Cannot add multiple application servers

### **Future Improvements:**
1. **Multi-Threading:**
   - Thread-safe database connections
   - Concurrent request handling
   - Background worker threads
   - Thread pool management

2. **Modern Memory Management:**
   - Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
   - RAII (Resource Acquisition Is Initialization)
   - Automatic memory management
   - Memory leak detection

3. **Distributed Architecture:**
   - Microservices architecture
   - RESTful API design
   - Service-oriented architecture (SOA)
   - API gateway implementation

4. **Scalability Solutions:**
   - Load balancing
   - Database replication (master-slave)
   - Sharding for large datasets
   - Caching layer (Redis cluster)

5. **Cloud Deployment:**
   - Docker containerization
   - Kubernetes orchestration
   - Auto-scaling capabilities
   - Cloud-native architecture

---

## **7. DATA MANAGEMENT CONSTRAINTS**

### **Current Limitations:**
- **No Data Archiving:** All historical data kept in same tables
- **No Soft Delete:** Hard delete only, cannot recover deleted records
- **Limited Data Validation:** Basic validation only
- **No Data Import/Export:** Cannot bulk import or export data
- **No Data Versioning:** Cannot track data changes over time
- **No Data Backup:** No automated backup system
- **Limited Data Integrity:** Basic foreign keys only

### **Future Improvements:**
1. **Data Archiving System:**
   - Archive old rentals to separate tables
   - Automated archiving rules
   - Data retention policies
   - Archive compression

2. **Soft Delete Implementation:**
   - Add `DeletedAt` timestamp column
   - Soft delete for all entities
   - Recycle bin functionality
   - Data recovery capability

3. **Data Versioning:**
   - Track all data changes (audit log)
   - Version history for records
   - Rollback to previous versions
   - Change tracking system

4. **Data Import/Export:**
   - CSV/Excel import wizard
   - Bulk data import
   - Scheduled exports
   - API for data exchange

5. **Enhanced Data Integrity:**
   - Database triggers for validation
   - Stored procedures for complex operations
   - Data integrity checks
   - Referential integrity enforcement

---

## **8. ERROR HANDLING CONSTRAINTS**

### **Current Limitations:**
- **Basic Error Handling:** Simple try-catch blocks
- **No Error Logging:** Errors only printed to console (`std::cerr`)
- **No Error Recovery:** Limited automatic recovery mechanisms
- **No Error Categories:** All errors handled similarly
- **No Error Notifications:** No alerts for critical errors
- **Limited Error Messages:** Generic error messages
- **No Error Tracking:** Cannot track error patterns

### **Future Improvements:**
1. **Comprehensive Error Logging:**
   - File-based error logging
   - Log levels (DEBUG, INFO, WARN, ERROR, FATAL)
   - Structured logging (JSON format)
   - Log rotation and archival

2. **Error Tracking System:**
   - Error aggregation and analysis
   - Error trend monitoring
   - Critical error alerts
   - Error dashboard

3. **Enhanced Error Handling:**
   - Custom exception classes
   - Error code system
   - Error recovery strategies
   - Graceful degradation

4. **User-Friendly Error Messages:**
   - Localized error messages
   - Detailed error descriptions
   - Error code lookup system
   - Helpful error suggestions

5. **Error Monitoring:**
   - Real-time error monitoring
   - Error notification system (email, SMS)
   - Error analytics and reporting

---

## **9. REPORTING CONSTRAINTS**

### **Current Limitations:**
- **Fixed Report Formats:** Predefined reports only
- **Limited Visualization:** ASCII charts only
- **No Custom Reports:** Cannot create custom reports
- **No Scheduled Reports:** Manual generation only
- **No Report Templates:** Fixed report format
- **Limited Data Export:** No export functionality
- **Hardcoded Limits:** Top 20 only, cannot customize
- **No Interactive Reports:** Static reports only

### **Future Improvements:**
1. **Custom Report Builder:**
   - Drag-and-drop report designer
   - Custom SQL query builder
   - Save and share reports
   - Report templates library

2. **Advanced Visualization:**
   - Interactive charts and graphs
   - Drill-down capabilities
   - Multi-dimensional analysis
   - Real-time dashboards

3. **Scheduled Reports:**
   - Automated report generation
   - Email report delivery
   - Report scheduling (daily, weekly, monthly)
   - Report subscription system

4. **Export Options:**
   - Multiple formats (PDF, Excel, CSV, JSON)
   - Customizable export templates
   - Bulk export functionality
   - API-based report access

5. **Interactive Analytics:**
   - OLAP (Online Analytical Processing)
   - Data mining capabilities
   - Predictive analytics
   - Business intelligence tools integration

---

## **10. FUNCTIONALITY CONSTRAINTS**

### **Current Limitations:**
- **No Email Notifications:** No email sending capability
- **No SMS Integration:** No SMS alerts
- **No Invoice Generation:** Basic receipt only
- **No Discount System:** No discount or promotion management
- **No Customer Loyalty Program:** Basic loyalty tracking only
- **No Maintenance Scheduling:** No automated maintenance reminders
- **No Reservation System:** No advance booking capability
- **No Payment Gateway Integration:** Manual payment entry only
- **No Multi-Currency Support:** Only RM (Malaysian Ringgit)

### **Future Improvements:**
1. **Notification System:**
   - Email notifications (rental reminders, overdue alerts)
   - SMS integration
   - Push notifications
   - In-app notifications

2. **Advanced Payment:**
   - Payment gateway integration (Stripe, PayPal)
   - Online payment processing
   - Payment plan support
   - Refund management

3. **Discount & Promotions:**
   - Discount code system
   - Seasonal promotions
   - Customer tier discounts
   - Bulk rental discounts

4. **Reservation System:**
   - Advance booking capability
   - Reservation calendar
   - Booking confirmation system
   - Waitlist management

5. **Internationalization:**
   - Multi-currency support
   - Multi-language support
   - Date/time format localization
   - Regional settings

6. **Additional Features:**
   - Customer loyalty points system
   - Referral program
   - Automated invoice generation
   - Maintenance scheduling system
   - Damage assessment tracking
   - Insurance claim management

---

## **11. CODE QUALITY CONSTRAINTS**

### **Current Limitations:**
- **Manual Memory Management:** Uses `new`/`delete` extensively (memory leak risk)
- **No Unit Testing:** No automated tests
- **Limited Code Documentation:** Basic comments only
- **No Code Standards Enforcement:** Inconsistent coding style
- **No Static Analysis:** No code quality tools
- **Limited Error Handling:** Basic exception handling
- **Code Duplication:** Some repeated code patterns

### **Future Improvements:**
1. **Modern C++ Practices:**
   - Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
   - RAII pattern
   - Move semantics
   - C++17/C++20 features

2. **Testing Framework:**
   - Unit testing (Google Test, Catch2)
   - Integration testing
   - Test coverage analysis
   - Continuous testing

3. **Code Quality:**
   - Static code analysis (Clang-Tidy, Cppcheck)
   - Code coverage tools
   - Code review process
   - Coding standards enforcement

4. **Documentation:**
   - Doxygen documentation
   - API documentation
   - User manuals
   - Developer guides

---

## **12. DEPLOYMENT CONSTRAINTS**

### **Current Limitations:**
- **Platform-Specific:** Requires compilation for each platform
- **Dependency Management:** Manual library installation
- **No Containerization:** Cannot use Docker
- **No CI/CD Pipeline:** Manual deployment only
- **No Version Management:** No versioning system for releases
- **Limited Distribution:** Source code distribution only

### **Future Improvements:**
1. **Containerization:**
   - Docker container support
   - Docker Compose for multi-container setup
   - Kubernetes deployment
   - Container orchestration

2. **CI/CD Pipeline:**
   - Automated testing on commit
   - Automated builds
   - Automated deployment
   - Version tagging

3. **Package Management:**
   - Binary distribution packages
   - Installer creation (Windows MSI, Linux DEB/RPM)
   - Package repositories
   - Dependency management system

4. **Deployment Automation:**
   - Infrastructure as Code (IaC)
   - Automated deployment scripts
   - Rollback capabilities
   - Blue-green deployment

---

## **SUMMARY OF PRIORITY IMPROVEMENTS**

### **High Priority:**
1. Configuration file system for database settings
2. Enhanced password security (bcrypt/Argon2)
3. Connection pooling
4. Pagination for large datasets
5. Soft delete implementation
6. Error logging to file
7. Smart pointers for memory management

### **Medium Priority:**
1. Web-based or GUI interface
2. Export functionality (PDF, Excel)
3. Email notifications
4. Configurable business rules
5. Data archiving system
6. Unit testing framework
7. Caching system

### **Low Priority:**
1. Multi-currency support
2. Multi-language support
3. Microservices architecture
4. Cloud deployment
5. Advanced analytics
6. Mobile app development


