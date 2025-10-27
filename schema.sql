BEGIN;


CREATE TABLE IF NOT EXISTS public.account_owners
(
    account_id uuid NOT NULL,
    customer_id uuid NOT NULL,
    ownership_percent numeric(5, 2) DEFAULT 100.00,
    is_primary boolean DEFAULT false,
    CONSTRAINT account_owners_pkey PRIMARY KEY (account_id, customer_id)
);

CREATE TABLE IF NOT EXISTS public.accounts
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    account_number character varying(34) COLLATE pg_catalog."default" NOT NULL,
    account_type text COLLATE pg_catalog."default" NOT NULL,
    currency character(3) COLLATE pg_catalog."default" NOT NULL DEFAULT 'RUB'::bpchar,
    status text COLLATE pg_catalog."default" NOT NULL,
    branch_id uuid,
    opened_by_employee_id uuid,
    created_at timestamp with time zone DEFAULT now(),
    CONSTRAINT accounts_pkey PRIMARY KEY (id),
    CONSTRAINT accounts_account_number_key UNIQUE (account_number)
);

CREATE TABLE IF NOT EXISTS public.audit_log
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    entity_type text COLLATE pg_catalog."default" NOT NULL,
    entity_id uuid,
    action text COLLATE pg_catalog."default" NOT NULL,
    performed_by uuid,
    performed_at timestamp with time zone DEFAULT now(),
    details jsonb,
    CONSTRAINT audit_log_pkey PRIMARY KEY (id)
);

CREATE TABLE IF NOT EXISTS public.branches
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    name text COLLATE pg_catalog."default" NOT NULL,
    address text COLLATE pg_catalog."default",
    swift_code text COLLATE pg_catalog."default",
    created_at timestamp with time zone DEFAULT now(),
    CONSTRAINT branches_pkey PRIMARY KEY (id)
);

CREATE TABLE IF NOT EXISTS public.cards
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    account_id uuid,
    card_type text COLLATE pg_catalog."default",
    status text COLLATE pg_catalog."default",
    last_four character(4) COLLATE pg_catalog."default",
    pan_encrypted bytea,
    expiry_date date,
    issued_at timestamp with time zone DEFAULT now(),
    CONSTRAINT cards_pkey PRIMARY KEY (id)
);

CREATE TABLE IF NOT EXISTS public.customers
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    first_name text COLLATE pg_catalog."default" NOT NULL,
    last_name text COLLATE pg_catalog."default" NOT NULL,
    dob date,
    email text COLLATE pg_catalog."default",
    phone text COLLATE pg_catalog."default",
    passport_encrypted bytea,
    created_at timestamp with time zone DEFAULT now(),
    CONSTRAINT customers_pkey PRIMARY KEY (id),
    CONSTRAINT customers_email_key UNIQUE (email)
);

CREATE TABLE IF NOT EXISTS public.employee_accounts
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    employee_id uuid,
    username text COLLATE pg_catalog."default" NOT NULL,
    full_name text COLLATE pg_catalog."default",
    password_hash bytea NOT NULL,
    salt bytea NOT NULL,
    role text COLLATE pg_catalog."default" NOT NULL,
    created_at timestamp with time zone DEFAULT now(),
    last_login timestamp with time zone,
    CONSTRAINT employee_accounts_pkey PRIMARY KEY (id),
    CONSTRAINT employee_accounts_username_key UNIQUE (username)
);

CREATE TABLE IF NOT EXISTS public.employees
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    first_name text COLLATE pg_catalog."default" NOT NULL,
    last_name text COLLATE pg_catalog."default" NOT NULL,
    role text COLLATE pg_catalog."default",
    email text COLLATE pg_catalog."default",
    branch_id uuid,
    hired_at date,
    CONSTRAINT employees_pkey PRIMARY KEY (id),
    CONSTRAINT employees_email_key UNIQUE (email)
);

CREATE TABLE IF NOT EXISTS public.transactions
(
    id uuid NOT NULL DEFAULT gen_random_uuid(),
    account_id uuid NOT NULL,
    tx_group uuid,
    type text COLLATE pg_catalog."default" NOT NULL,
    amount numeric(18, 2) NOT NULL,
    currency character(3) COLLATE pg_catalog."default" NOT NULL,
    performed_by_employee uuid,
    created_at timestamp with time zone DEFAULT now(),
    description text COLLATE pg_catalog."default",
    metadata jsonb DEFAULT '{}'::jsonb,
    CONSTRAINT transactions_pkey PRIMARY KEY (id)
);

ALTER TABLE IF EXISTS public.account_owners
    ADD CONSTRAINT account_owners_account_id_fkey FOREIGN KEY (account_id)
    REFERENCES public.accounts (id) MATCH SIMPLE
    ON UPDATE NO ACTION
    ON DELETE CASCADE;


ALTER TABLE IF EXISTS public.account_owners
    ADD CONSTRAINT account_owners_customer_id_fkey FOREIGN KEY (customer_id)
    REFERENCES public.customers (id) MATCH SIMPLE
    ON UPDATE NO ACTION
    ON DELETE CASCADE;


ALTER TABLE IF EXISTS public.accounts
    ADD CONSTRAINT accounts_branch_id_fkey FOREIGN KEY (branch_id)
    REFERENCES public.branches (id) MATCH SIMPLE
    ON UPDATE NO ACTION
    ON DELETE NO ACTION;


ALTER TABLE IF EXISTS public.cards
    ADD CONSTRAINT cards_account_id_fkey FOREIGN KEY (account_id)
    REFERENCES public.accounts (id) MATCH SIMPLE
    ON UPDATE NO ACTION
    ON DELETE CASCADE;
CREATE INDEX IF NOT EXISTS cards_account_id_idx
    ON public.cards(account_id);


ALTER TABLE IF EXISTS public.employee_accounts
    ADD CONSTRAINT employee_accounts_employee_id_fkey FOREIGN KEY (employee_id)
    REFERENCES public.employees (id) MATCH SIMPLE
    ON UPDATE NO ACTION
    ON DELETE NO ACTION;


ALTER TABLE IF EXISTS public.employees
    ADD CONSTRAINT employees_branch_id_fkey FOREIGN KEY (branch_id)
    REFERENCES public.branches (id) MATCH SIMPLE
    ON UPDATE NO ACTION
    ON DELETE NO ACTION;


ALTER TABLE IF EXISTS public.transactions
    ADD CONSTRAINT transactions_account_id_fkey FOREIGN KEY (account_id)
    REFERENCES public.accounts (id) MATCH SIMPLE
    ON UPDATE NO ACTION
    ON DELETE CASCADE;
CREATE INDEX IF NOT EXISTS transactions_account_id_idx
    ON public.transactions(account_id);

END;