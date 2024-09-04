FROM ubuntu:20.04 AS build-env

# Set the timezone
ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Configure env
ARG SECRET_KEY
ARG DB_HOST
ARG DB_PORT
ARG DB_NAME
ARG DB_USER
ARG DB_PASSWORD

ENV SECRET_KEY=$SECRET_KEY
ENV DB_HOST=$DB_HOST
ENV DB_PORT=$DB_PORT
ENV DB_NAME=$DB_NAME
ENV DB_USER=$DB_USER
ENV DB_PASSWORD=$DB_PASSWORD

# Update and install necessary packages
RUN apt-get update
RUN apt-get install -y redis-server libhiredis-dev git
RUN apt-get install -y cmake g++ gcc libjsoncpp-dev uuid-dev openssl
RUN apt-get install -y libssl-dev zlib1g-dev libbz2-dev liblzma-dev
RUN apt-get install -y postgresql postgresql-contrib postgresql-all
RUN apt clean && rm -rf /var/lib/apt/lists/*

# Clone the Drogon repository
RUN git clone https://github.com/drogonframework/drogon

# Build and install the Drogon library
RUN cd drogon && \
    git submodule update --init && \
    mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make && make install

# Clone JWT-CPP repository
RUN git clone https://github.com/Thalhammer/jwt-cpp.git

# Build and install the JWT-CPP library
RUN cd jwt-cpp && mkdir build && cd build && cmake .. && make && make install

# Copy the application code
COPY . .

# Install brcrypt
RUN git clone https://github.com/hilch/Bcrypt.cpp.git

# Build app
RUN cmake . && make && chmod +x drogon_user_service

# Expose port 8000 for the app
EXPOSE 8000

# Start the app
CMD ["./drogon_user_service", "--action=run-server"]
