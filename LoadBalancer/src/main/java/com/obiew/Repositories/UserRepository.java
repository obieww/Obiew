package com.obiew.Repositories;

import com.obiew.Entities.User;
import org.springframework.data.repository.CrudRepository;

public interface UserRepository extends CrudRepository<User, String> {
    User findByUsername(String username);
    User findByUserId(String userId);
}
