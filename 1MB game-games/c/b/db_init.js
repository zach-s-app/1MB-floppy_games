// Virtual File System (VFS) Mapping
const initDB = async (sqlite3) => {
  const db = new sqlite3.oo1.DB("shadow_seed.db", 'c');
  db.exec("CREATE TABLE IF NOT EXISTS inventory (id INTEGER PRIMARY KEY, item TEXT, power INT)");
  db.exec("INSERT INTO inventory (item, power) VALUES ('Shadow Dagger', 15)");
  return db;
};
