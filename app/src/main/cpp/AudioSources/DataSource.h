//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_DATASOURCE_H
#define OBOELIBTEST_DATASOURCE_H

class DataSource {
public:
    virtual ~DataSource(){};
    virtual int32_t getTotalFrames() const = 0;
    virtual int32_t getChannelCount() const  = 0;
    virtual const float* getData() const = 0;

};


#endif //OBOELIBTEST_DATASOURCE_H
