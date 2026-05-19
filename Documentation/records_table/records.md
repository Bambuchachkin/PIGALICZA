# Creating records table

## MySQL

```
sudo mysql
USE pigalicza_db
```
```
CREATE TABLE tanks_records(
    user_id INT PRIMARY KEY,
    score INT NOT NULL DEFAULT 0,
    time INT NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

```
CREATE TABLE races_records(
    user_id INT PRIMARY KEY,
    score INT NOT NULL DEFAULT 0,
    time INT NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

```
ALTER TABLE users
ADD COLUMN verification_code UNIQUE;
```