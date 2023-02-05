#include "../include/data_handler.hpp"

    data_handler::data_handler()
    {
        data_array = new std::vector<data *>;
        training_data = new std::vector<data *>;
        test_data = new std::vector<data *>;
        validation_data = new std::vector<data *>;
    };
    data_handler::~data_handler()
    {
        //Liberar espaco alocdo;
    };

    void data_handler::read_feature_vector(std::string path)
    {
        uint32_t header[4]; // Numero Magico, qtd imagens, qtd linhas, qtd_colunas
        unsigned char bytes[4];
        
        FILE *f = fopen(path.c_str(), "rb");

        if (f) {
            for (int i = 0; i < 4 ; i++){
                if(fread(bytes, sizeof(bytes), 1, f)){
                    header[i] = convert_to_little_endian(bytes);
                }
            }

            printf("Abriu o arquivo de imagem corretamente.\n");

            int image_size = header[2] * header [3];

            for(int i = 0; i < header[1]; i++) {
                data *d = new data();
                uint8_t element[1];

                for(int j = 0; j < image_size; j++) {
                    if (fread(element, sizeof(element), 1, f)){
                        d->append_to_feature_vector(element[0]);
                    } 
                    else {
                        printf("Erro ao ler o arquivo.\n");
                        exit(1);
                    }
                }
                data_array->push_back(d);
            }
            printf("Leu e gravou %lu Features com sucesso.\n", data_array->size());
        } else {
            printf("Arquivo nao encontrado.\n");
            exit(1);
        }

    };

    void data_handler::read_feature_labels(std::string path)
    {
        uint32_t header[2]; // Numero Magico, qtd imagens
        unsigned char bytes[4];
        
        FILE *f = fopen(path.c_str(), "rb");

        if (f) {
            for (int i = 0; i < 2 ; i++){
                if(fread(bytes, sizeof(bytes), 1, f)){
                    header[i] = convert_to_little_endian(bytes);
                }
            }

            printf("Abriu o arquivo de labels corretamente.\n");

            for(int i = 0; i < header[1]; i++) {
                uint8_t element[1];

                if (fread(element, sizeof(element), 1, f)){
                    data_array->at(i)->set_label(element[0]);
                } 
                else {
                    printf("Erro ao ler o arquivo.\n");
                    exit(1);
                }
            }
            printf("Leu e gravou %lu Labels com sucesso.\n", data_array->size());
        } else {
            printf("Arquivo nao encontrado.\n");
            exit(1);
        }
    };

    void data_handler::split_data()
    {
        std::unordered_set<int> used_indexes;

        int train_size = data_array->size() * TRAIN_SET_PERCENT;
        int test_size = data_array->size() * TEST_SET_PERCENT;
        int validation_size = data_array->size() * VALIDATION_SET_PERCENT;

        // Train data

        int count = 0;

        while(count < train_size) {
            int random_index = rand() % data_array->size(); // indice aleatorio entre 0 e tamanho do array -1 

            if(used_indexes.find(random_index) == used_indexes.end()){ // Se o indice aleatorio nao estiver no final do set de indices utilizados ->
                training_data->push_back(data_array->at(random_index)); // Inserir este valor correspondente ao indice no vetor de treinamento;
                used_indexes.insert(random_index);
                count++;
            }
        }

        // Test data

        count = 0;

        while(count < test_size) {
            int random_index = rand() % data_array->size(); // indice aleatorio entre 0 e tamanho do array -1 

            if(used_indexes.find(random_index) == used_indexes.end()){ // Se o indice aleatorio nao estiver no final do set de indices utilizados ->
                test_data->push_back(data_array->at(random_index)); // Inserir este valor correspondente ao indice no vetor de treinamento;
                used_indexes.insert(random_index);
                count++;
            }
        }

        // Validation data

        count = 0;

        while(count < validation_size) {
            int random_index = rand() % data_array->size(); // indice aleatorio entre 0 e tamanho do array -1 

            if(used_indexes.find(random_index) == used_indexes.end()){ // Se o indice aleatorio nao estiver no final do set de indices utilizados ->
                validation_data->push_back(data_array->at(random_index)); // Inserir este valor correspondente ao indice no vetor de treinamento;
                used_indexes.insert(random_index);
                count++;
            }
        }


        printf("Tamanho de dados de treinamento: %lu.\n", training_data->size());
        printf("Tamanho de dados de teste: %lu.\n", test_data->size());
        printf("Tamanho de dados de validacao: %lu.\n", validation_data->size());

    };

    void data_handler::count_classes()
    {
        int count = 0;

        for(unsigned i = 0; i < data_array->size(); i++) {
            if(class_map.find(data_array->at(i)->get_label()) == class_map.end()) {
                class_map[data_array->at(i)->get_label()] = count;
                data_array->at(i)->set_enumerated_label(count);
                count++;
            }
        }
        num_classes = count;
        printf("Extraido com sucesso %d classes unicas.\n", num_classes);
    };

    uint32_t data_handler::convert_to_little_endian(const unsigned char* bytes)
    {
        return (uint8_t) (
            (bytes[0] << 24) | //Mover o primeiro byte 24 bits para a esquerda, tornando se o terceiro byte.
            (bytes[1] << 16) | //Mover o segundo byte 16 bits para a esquerda,  tornando se se o segundo byte.
            (bytes[2] << 8)  | //Mover o terceiro byte 8 bytes para a esquerda, tornando se o primeiro byte.
            (bytes[3] << 0)    //Ultimo byte permanece inalterado, pois ja movemos os outros 3 bytes.
            );
    };

    std::vector<data *> * data_handler::get_training_data()
    {
        return training_data;
    };
    std::vector<data *> * data_handler::get_test_data()
    {
        return test_data;
    };
    std::vector<data *> * data_handler::get_validation_data()
    {
        return validation_data;
    };

    int main()
    {
        data_handler *dh = new data_handler();

        dh->read_feature_vector("./train-images-idx3-ubyte");
        dh->read_feature_labels("./train-labels-idx1-ubyte");

        dh->split_data();
        dh->count_classes();
    }
