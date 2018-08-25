const mongo = require('mongodb');
const MongoClient = require('mongodb').MongoClient;

module.exports = class MongoDataClient {

/**
 * Creates an instance of MongoDataClient.
 * @param {string} url 
 * @param {string} [database=null] 
 */
constructor(url, database = null) {
        this.url = url;
        this.database = database;
        this.SINGLE_DATA_PACKAGE_COLLECTION = "DataPackage"
        //this._start(this.url);
    }

    /**
     * 
     * 
     * @returns {Promise<MongoDataClient>}
     * Create DataStore
     */
    async start() {
        let self = this;
        const client = await MongoClient.connect(self.url)

        let dbo = client.db(self.database || 't4ldb');
        let collection = await dbo.createCollection(self.SINGLE_DATA_PACKAGE_COLLECTION);

        client.close();
        return this;
    }

    
    /**
     * 
     * @param {any} values 
     * @returns {Promise<MongoDataClient>}
     *  Insert Single Data Packet
     */
    async insertOne(value) {
        let self = this;
        let client = await MongoClient.connect(this.url);
        let dbo = client.db(self.database || 't4ldb');
        let myobj = {
            timestamp: Date.now(),
            data: value
        };
        let collection = await dbo.collection(self.SINGLE_DATA_PACKAGE_COLLECTION).insertOne(myobj)
        client.close();

        return this;
    }

    /**
     * 
     * @param {any[]} values 
     * @returns {Promise<MongoDataClient>}
     *  Insert Collection of Single Data Packet
     */
    async insertMany(values) {
        let self = this;
        let client = await MongoClient.connect(self.url)
        let dbo = client.db(self.database || 't4ldb');

        let chuncks = [];
        values.forEach(element => {
            chuncks.push({
                timestamp: Date.now(),
                data: element
            });
        });
        let collection = await dbo.collection(self.SINGLE_DATA_PACKAGE_COLLECTION).insertMany(chuncks)
        client.close();
        return this;
    }

    /**
     * 
     * @param {number} quantity
     * @returns {any[]}
     * 
     * Returns Promise with the latests Data Documents 
     */
    async collectData(quantity) {
        let chunk = [];
        let self = this;
        let client = await MongoClient.connect(self.url)
        let dbo = client.db(self.database || 't4ldb');


        let res = await dbo.collection(self.SINGLE_DATA_PACKAGE_COLLECTION)
            .find({})
            .toArray()
        for (let i = 0; i < quantity; i++) {
            chunk.push(res[i]);
        }
        client.close();
        return chunk;
    }

    /**
     * 
     * @param {number} quantity 
     * @returns {Promise<MongoDataClient>}
     * Delete latests ocurrence top to botton of Collection
     */
    async deleteLatests(quantity) {
        let self = this;
        try {
            const client = await MongoClient.connect(self.url);
            let dbo = client.db(self.database || 't4ldb');

            let res = await dbo.collection(self.SINGLE_DATA_PACKAGE_COLLECTION)
                .find({})
                .toArray()

            let collection;
            for (let i = 0; i < quantity; i++) {
                collection = await dbo.collection(self.SINGLE_DATA_PACKAGE_COLLECTION).deleteOne(res[i])
            }
            client.close();
        } catch (err) {
            console.error(err);
        }
        return this;
    }

    // Create Custom Data Collection

    // Insert Custom Data Packet


}