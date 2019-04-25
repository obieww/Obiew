package com.obiew.Repositories;

import com.obiew.Entities.Obiew;
import org.springframework.data.repository.CrudRepository;

public interface ObiewRepository extends CrudRepository<Obiew, String> {
    Obiew findByObiewId(String obiewId);
}
